#ifndef BEHAVIOURAL_SRAMMEMORY_H_
#define BEHAVIOURAL_SRAMMEMORY_H_

#include <string>
#include <vector>
#include "../structural/SRAMMemorySIM.h"
#include "MemI.h"

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
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();
    // check address range
    if (adr >= static_cast<sc_dt::uint64>(SIZE) ||
        byt != 0 || len > 4 || wid < len) {
      std::cerr << "tlm_memory " << modulename
            << " Target does not support given mem payload transaction addr:"
            <<adr << endl;
      std::cerr << "tlm_memory - You probably have an out of range address"
            <<endl;
      SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");  // NOLINT(whitespace/line_length)
      sc_stop();
    }
    // Read or Write depending upon command.
    if ( cmd == tlm::TLM_READ_COMMAND ) {
      //memcpy(ptr, &mem[adr], len);
      if (mem.find(adr) != mem.end()) {
          std::deque<sc_dt::uint64>::iterator it = dq.begin();
            while (*it != adr) {
                it++;
            }
            // update queue: update it to most recent used value
            dq.erase(it);
            dq.push_front(adr);
            // return m[key];
            //memcpy(ptr, &mem[adr], len);
            ptr = mem[adr];
        outlog<<"READ Command, addr: "<<adr<<endl;  // NOLINT
      }
    } else if ( cmd == tlm::TLM_WRITE_COMMAND ) {
      //memcpy(&mem[adr], ptr, len);
      if(mem.find(adr) == mem.end()) {
          // check if cache is full
          if (SIZE == dq.size()) {
              sc_dt::uint64 last = dq.back();
              dq.pop_back();
              mem.erase(last);
          } 
        } else {
            std::deque<sc_dt::uint64>::iterator it = dq.begin();
            while (*it != adr) {
                it++;
            }
            dq.erase(it);
            mem.erase(adr);
        }
        dq.push_front(adr);
        //memcpy(mem[adr], ptr, len);
        mem[adr] = ptr;
      outlog<<"WRITE Command, addr: "<<adr<<endl;  // NOLINT
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
  std::unordered_map<sc_dt::uint64, unsigned char*> mem;
  std::deque<sc_dt::uint64> dq;
  std::ofstream outlog;

};

/*
  Memory Consturctor
 */
template<typename T>
SRAMMemory<T>::SRAMMemory
  (sc_module_name nm, pfp::core::PFPObject* parent, std::string configfile)
  : SRAMMemorySIM(nm, parent, configfile),
  outlog(OUTPUTDIR+"SRAMMemoryTrace.csv") {
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
  mem.clear();
  dq.clear();
}

#endif  // BEHAVIOURAL_MEMORY_H_
