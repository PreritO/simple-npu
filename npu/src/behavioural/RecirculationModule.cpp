#include "./RecirculationModule.h"
#include <string>
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
      cout << "RM: GOT Recirculation PACKET: " << received_pd->id() << ". recirc sent at: " <<received_pd->get_packet_time_recirc_() << endl;
      JobsReceived.push(received_pd);
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
      // Treat in a FIFO fashion..
      auto job = JobsReceived.front();
      // Prerit TODO: Update this to not be hardcorded, instead get this from a configuration file
      if ((sc_time_stamp().to_default_time_units() - job->get_packet_time_recirc_()) > 100) {
        JobsReceived.pop();
        cout << "RM: sending back to parser" << endl;
        ocn_wr_if->put(make_routing_packet<PacketDescriptor>(
          module_name(),
          "parser",
          job));
      } else {
        wait(GotaJob);
      }
    } else {
      wait(GotaJob);
    }
  }
}
