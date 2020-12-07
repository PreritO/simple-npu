#include "./Accelerator.h"
#include <string>
#include "common/RoutingPacket.h"
#include "common/Packet.h"
#include "common/PacketDescriptor.h"
#include "common/RoutingPacket.h"
#include "common/routingdefs.h"
extern "C" {
  #include "hash.h"
}
Accelerator::Accelerator(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):AcceleratorSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
  ThreadHandles.push_back(
      sc_spawn(sc_bind(&Accelerator::Accelerator_PortServiceThread, this)));
}

void Accelerator::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void Accelerator::Accelerator_PortServiceThread() {
  while (1) {
    auto received_tr = ocn_rd_if->get();
     if (auto received_rp = try_unbox_routing_packet
                          <IPC_MEM>(received_tr)) {
        auto ipcpkt = received_rp->payload;
        if (ipcpkt->RequestType == "ACCEL_ASYNC") {
          npulog(cout<<"RCVD MESSAGE AT ACCEL!"<<endl;)
          job_buffer_.push(ipcpkt);
          newMessage.notify();
        }
      }
  }
  // Thread function to service input ports.
}
void Accelerator::AcceleratorThread(std::size_t thread_id) {
  // Thread function for module functionalty
  while(1) {
    wait(newMessage);
    // table size and seeds should already be stored here. 
    cout << "ACCEL THREAD!" << endl;
    // now, hash the desired key (send in payload->rightKey) to four addresses

  }
}
