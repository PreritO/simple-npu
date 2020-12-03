#ifndef BEHAVIOURAL_SRAMMEMORY_H_
#define BEHAVIOURAL_SRAMMEMORY_H_

#include <string>
#include <vector>
#include "../structural/SRAMMemorySIM.h"
#include "MemI.h"
#include "lru_cache.hpp"

// #define debug_tlm_mem_transaction 1
template<typename T>
class SRAMMemory:
public MemI<T>,
public SRAMMemorySIM {
 public:
  /* CE Consturctor */
  SRAMMemory(sc_module_name nm,
         pfp::core::PFPObject* parent = 0,
         std::string configfile = "");
  /* User Implementation of the Virtual Functions in the Interface.h file */
  virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay) {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   dataptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();
    // check address range. Commenting this out because SRAM Memory here is no longer a 
    // continuous vector, it's not an unordered map where map key = address and map value = ptr - PO

    // if (adr >= static_cast<sc_dt::uint64>(SIZE) ||
    //     byt != 0 || len > 4 || wid < len) {
    //   std::cerr << "tlm_memory " << modulename
    //         << " Target does not support given mem payload transaction addr:"
    //         <<adr << endl;
    //   std::cerr << "tlm_memory - You probably have an out of range address"
    //         <<endl;
    //   SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");  // NOLINT(whitespace/line_length)
    //   sc_stop();
    // }
    // Read or Write depending upon command.
    if ( cmd == tlm::TLM_READ_COMMAND ) {
      unsigned int data = 0;
      if(cache.exists(adr)) {
        data = cache.get(adr);
      }
      memcpy(dataptr, &data, len);
        //outlog<<"READ Command, addr: "<<adr<<endl;  // NOLINT
    } else if ( cmd == tlm::TLM_WRITE_COMMAND ) {
      unsigned tmp;
      memcpy(&tmp, dataptr, len);
      cache.put(adr, tmp);
      //outlog<<"WRITE Command, addr: "<<adr<<endl;  // NOLINT
    }
    // response status to indicate successful completion
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
  }

 private:
  std::string modulename;
  //std::vector<int> mem;
  uint64_t SIZE;
  sc_time RD_LATENCY;
  sc_time WR_LATENCY;
  //std::unordered_map<sc_dt::uint64, unsigned char*> mem;
  //std::deque<sc_dt::uint64> dq;
  //std::ofstream outlog;
  cache::lru_cache<sc_dt::uint64, unsigned int> cache;
};

/*
  Memory Consturctor
 */
template<typename T>
SRAMMemory<T>::SRAMMemory
  (sc_module_name nm, pfp::core::PFPObject* parent, std::string configfile)
  : SRAMMemorySIM(nm, parent, configfile) {
  int rdlt = GetParameter("ReadLatency").template get<int>();
  int wrlt = GetParameter("WriteLatency").template get<int>();

  sc_time rd_lat(rdlt, SC_NS);
  sc_time wr_lat(wrlt, SC_NS);
  int mem_size = GetParameter("Capacity").template get<int>();
  RD_LATENCY = rd_lat;
  WR_LATENCY = wr_lat;
  SIZE = mem_size;
  // Initialize memory with random data
  //   for (int i = 0; i < SIZE; i++) {
  //     mem.push_back(0xAA000000);
  //   }
  cache = cache::lru_cache<sc_dt::uint64, unsigned int>(SIZE);
}

#endif  // BEHAVIOURAL_MEMORY_H_
