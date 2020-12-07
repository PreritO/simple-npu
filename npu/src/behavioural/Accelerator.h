#ifndef BEHAVIOURAL_ACCELERATOR_H_
#define BEHAVIOURAL_ACCELERATOR_H_
#include <string>
#include <vector>
#include "../structural/AcceleratorSIM.h"
#include "common/RoutingPacket.h"
#include "common/routingdefs.h"
#include "common/IPC_MEM.h"
#include "common/MemoryUtility.h"

class Accelerator: public AcceleratorSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(Accelerator);
  /*Constructor*/
  explicit Accelerator(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~Accelerator() = default;

 public:
  void init();

 private:
  void Accelerator_PortServiceThread();
  void AcceleratorThread();
  void halAcceleratorThread();

  std::vector<sc_process_handle> ThreadHandles;
  std::queue<std::shared_ptr<IPC_MEM>> cpagent_buffer_;
  std::queue<std::shared_ptr<RoutingPacket<IPC_MEM>>> hal_buffer_;
  sc_mutex cpagent_buffer_mtx, hal_buffer_mtx;

  sc_event cpAgentMessage, halMessage;
  //Function that we are using the hash the addresses

  uint64_t seed1;
  uint64_t seed2;
  uint64_t table_size;
};

#endif  // BEHAVIOURAL_ACCELERATOR_H_
