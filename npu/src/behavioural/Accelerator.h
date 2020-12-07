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
  void AcceleratorThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
  std::queue<std::shared_ptr<IPC_MEM>> job_buffer_;
  sc_event newMessage;
  //Function that we are using the hash the addresses
};

#endif  // BEHAVIOURAL_ACCELERATOR_H_
