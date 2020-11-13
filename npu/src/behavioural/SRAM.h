#ifndef BEHAVIOURAL_SRAM_H_
#define BEHAVIOURAL_SRAM_H_
#include <string>
#include <vector>
#include "../structural/SRAMSIM.h"

class SRAM: public SRAMSIM {  // NOLINT(whitespace/line_length)
 public:
  SC_HAS_PROCESS(SRAM);
  /*Constructor*/
  explicit SRAM(sc_module_name nm, pfp::core::PFPObject* parent = 0, std::string configfile = "");  // NOLINT(whitespace/line_length)
  /*Destructor*/
  virtual ~SRAM() = default;

 public:
  void init();

 private:
  void SRAM_PortServiceThread();
  void SRAMThread(std::size_t thread_id);
  std::vector<sc_process_handle> ThreadHandles;
};

#endif  // BEHAVIOURAL_SRAM_H_
