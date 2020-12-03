//
// Created by kamil on 05/05/15.
//

#ifndef BEHAVIOURAL_COMMON_APPLICATIONREGISTRY_HPP
#define BEHAVIOURAL_COMMON_APPLICATIONREGISTRY_HPP

#include "Packet.h"
#include "PacketDescriptor.h"
#include <functional>
#include <map>
#include <string>

typedef std::function<Packet&(uint32_t&, PacketDescriptor&, Packet&, void*)>
    application_t;

typedef std::function<PacketDescriptor&(uint32_t&, PacketDescriptor&, void*)>
    application_pd_t;

inline std::map<std::string, application_t>::mapped_type& get_map_ref
    (const std::string &name) {
  static std::map<std::string, application_t> s_functions;
  return s_functions[name];
}

inline std::map<std::string, application_pd_t>::mapped_type& get_map_ref_pd
    (const std::string &name) {
  static std::map<std::string, application_pd_t> s_functions;
  return s_functions[name];
}

inline int register_application(const std::string &name, application_t app) {
    get_map_ref(name) = app;
    return 0;
}

inline int register_application_pd(const std::string &name, application_pd_t app) {
    get_map_ref_pd(name) = app;
    return 0;
}

#define call_application(app_name) get_map_ref(app_name)
#define call_application_pd(app_name) get_map_ref_pd(app_name)

#endif  // BEHAVIOURAL_COMMON_APPLICATIONREGISTRY_HPP
