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
  ThreadHandles.push_back(
      sc_spawn(sc_bind(&Accelerator::AcceleratorThread, this)));
  ThreadHandles.push_back(
      sc_spawn(sc_bind(&Accelerator::halAcceleratorThread, this)));
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
        if (ipcpkt->RequestType == "TABLE_UPDATE") {
          npulog(cout<<"RCVD MESSAGE AT ACCEL FROM CPAGENT!"<<endl;)
          cpagent_buffer_mtx.lock();
          cpagent_buffer_.push(ipcpkt);
          cpagent_buffer_mtx.unlock();
          cpAgentMessage.notify();
        } else if(ipcpkt->RequestType == "ACCEL_ASYNC") {
            npulog(cout<<"RCVD MESSAGE AT ACCEL FROM HAL!"<<endl;)
            hal_buffer_mtx.lock();
            hal_buffer_.push(received_rp);
            hal_buffer_mtx.unlock();
            halMessage.notify();
        } else if(ipcpkt->RequestType == "TABLE_READ") {
          received_rp->payload->seed1 = seed1;
          received_rp->payload->seed2 = seed1;
          received_rp->payload->table_size = table_size;
          received_rp->payload->RequestType = "TABLE_READ_RESPONSE";
          received_rp->destination = received_rp->source;
          received_rp->source = module_name_;
          ocn_wr_if->put(received_rp);
        }
      }
  }
  // Thread function to service input ports.
}
void Accelerator::AcceleratorThread() {
  // Thread function for module functionalty
  while(1) {
    wait(cpAgentMessage);
    auto ipcpkt = cpagent_buffer_.front();
    cpagent_buffer_mtx.lock();
    cpagent_buffer_.pop();
    cpagent_buffer_mtx.unlock();
    seed1 = ipcpkt->seed1;
    seed2 = ipcpkt->seed2;
    table_size = ipcpkt->table_size;
    // now, hash the desired key (send in payload->rightKey) to four addresses
  }
}

void Accelerator::halAcceleratorThread() {
  while(1) {
    wait(halMessage);
    auto ipcpkt = hal_buffer_.front();
    hal_buffer_mtx.lock();
    hal_buffer_.pop();
    hal_buffer_mtx.unlock();

    std::vector<bool> vec = ipcpkt->payload->rightKey.getVector();
    uint64_t hash1 = customHash((void *)(&vec), vec.size(), seed1);
    uint64_t hash2 = customHash((void *)(&vec), vec.size(), seed2);
    uint64_t addr1 = hash1%(table_size/2);
    uint64_t addr2 = hash2%(table_size/2)+(table_size/2);
    uint64_t addr3 = hash1%(table_size/4);
    uint64_t addr4 = hash2%(table_size/4)+(table_size/4);

    ipcpkt->payload->RequestType = "ACCEL_RESPONSE";
    ipcpkt->payload->addr[0] = addr1;
    ipcpkt->payload->addr[1] = addr2;
    ipcpkt->payload->addr[2] = addr3;
    ipcpkt->payload->addr[3] = addr4;

    ipcpkt->destination = ipcpkt->source;
    ipcpkt->source = module_name_;
    ocn_wr_if->put(ipcpkt);
  }
}
