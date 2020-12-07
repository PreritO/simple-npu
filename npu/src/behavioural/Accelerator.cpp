#include "./Accelerator.h"
#include <string>

Accelerator::Accelerator(sc_module_name nm, pfp::core::PFPObject* parent,std::string configfile ):AcceleratorSIM(nm,parent,configfile) {  // NOLINT(whitespace/line_length)
  /*sc_spawn threads*/
}

void Accelerator::init() {
  init_SIM(); /* Calls the init of sub PE's and CE's */
}
void Accelerator::Accelerator_PortServiceThread() {
  // Thread function to service input ports.
}
void Accelerator::AcceleratorThread(std::size_t thread_id) {
  // Thread function for module functionalty
}
