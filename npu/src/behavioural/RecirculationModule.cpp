#include "./RecirculationModule.h"
#include <string>
#include <vector>
#include <chrono>

#include "common/RoutingPacket.h"


RecirculationModule::RecirculationModule(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):RecirculationModuleSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
  ThreadHandles.push_back(sc_spawn(sc_bind(&RecirculationModule::RecirculationModule_PortServiceThread, this)));
  ThreadHandles.push_back(sc_spawn(sc_bind(&RecirculationModule::RecirculationModuleThread, this, 1)));
}

void RecirculationModule::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void RecirculationModule::RecirculationModule_PortServiceThread() {
  // Thread function to service input ports.
  while(1) {
    auto received_tr = ocn_rd_if->get();
    if (auto received_rp = try_unbox_routing_packet
                       <PacketDescriptor>(received_tr)) {
      auto received_pd = received_rp->payload;
      cout << "RM: GOT Recirculation PACKET FROM HAL: " << received_pd->id() << ". recirc sent at: " <<received_pd->get_packet_time_recirc_() << endl;
      jobsReceived_mtx.lock();
      JobsReceived.push(received_pd);
      jobsReceived_mtx.unlock();
      GotaJob.notify();
    } else {
      npu_error(module_name()
                +" Can only handle a PacketDescriptors");
    }
  }
}
void RecirculationModule::RecirculationModuleThread(std::size_t thread_id) {
  // Thread function for module functionalty
  while(1) {
    if (!JobsReceived.empty()) {
      // Treat in a FIFO fashion
      auto pd = JobsReceived.front();
      sc_time currentTime = sc_time_stamp();
      int imagDur = 125; // eventually make this a runtime variable from a configuration file. make this 2000 for TEA
      sc_time scImageDur =  currentTime + sc_time(imagDur, SC_NS); //< calculate the end time.
      // Prerit TODO: Update time difference to not be hardcorded, instead get this from a configuration file
      while((sc_time_stamp()) < scImageDur) {
            wait(10, SC_NS); //< Move simulation time. (Very Important.)
        }
      jobsReceived_mtx.lock();
      JobsReceived.pop();
      jobsReceived_mtx.unlock();
      auto sendtoparser = make_routing_packet(module_name(), "parser", pd);
      // 2. Check if it can write to the port or not.
      if (!ocn_wr_if->nb_can_put()) {
        // 2.1 Drop the PD.
        //cout << "RM Can't write, output fifo is full" << endl;
        // 2.2 Wait till space is avaliable in FIFO
        wait(ocn_wr_if->ok_to_put());
      }
      cout << "RM: FORWARDING pkt: " << pd->id() << "from " << sendtoparser->source << " to: " << sendtoparser->destination << endl;
      ocn_wr_if->put(sendtoparser);
      // JobsReceived.pop();
      // cout << "RM: sending back to scheduler" << endl;
      // auto sendtoparser = make_routing_packet(module_name(), "scheduler", pd);
      // ocn_wr_if->put(sendtoparser);
    } else {
        wait(GotaJob);
    }
  }
}
