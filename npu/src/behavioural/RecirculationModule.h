#ifndef BEHAVIOURAL_RECIRCULATIONMODULE_H_
#define BEHAVIOURAL_RECIRCULATIONMODULE_H_
#include <string>
#include <vector>
#include "../structural/RecirculationModuleSIM.h"
#include "CommonIncludes.h"


class RecirculationModule: public RecirculationModuleSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(RecirculationModule);
  /*Constructor*/
  explicit RecirculationModule(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~RecirculationModule() = default;

 public:
  void init();

 private:
  void RecirculationModule_PortServiceThread();
  void RecirculationModuleThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;

  //! Internal buffer to store received Jobs from HAL that need to be sent to parser again..
  std::queue<std::shared_ptr<PacketDescriptor>> JobsReceived;
  sc_event GotaJob;
  sc_mutex jobsReceived_mtx;
  sc_time now;
};

#endif  // BEHAVIOURAL_RECIRCULATIONMODULE_H_
