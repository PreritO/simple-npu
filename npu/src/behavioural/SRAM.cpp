#include "./SRAM.h"
#include <string>

SRAM::SRAM(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):SRAMSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
}

void SRAM::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void SRAM::SRAM_PortServiceThread() {
  // Thread function to service input ports.
}
void SRAM::SRAMThread(std::size_t thread_id) {
  // Thread function for module functionalty
}
