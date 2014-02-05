//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include <ebbrt/GlobalIdMap.h>

#include <ebbrt/Debug.h>
#include <ebbrt/EventManager.h>
#include <ebbrt/ExplicitlyConstructed.h>

namespace {
ebbrt::ExplicitlyConstructed<ebbrt::GlobalIdMap> the_map;
}

void ebbrt::GlobalIdMap::Init() { the_map.construct(); }

ebbrt::GlobalIdMap& ebbrt::GlobalIdMap::HandleFault(EbbId id) {
  kassert(id == kGlobalIdMapId);
  auto& ref = *the_map;
  EbbRef<GlobalIdMap>::CacheRef(id, ref);
  return ref;
}

void ebbrt::GlobalIdMap::Connect(uint32_t addr, uint16_t port) {
  ip_addr_t ip;
  ip.addr = htonl(addr);

  EventManager::EventContext context;
  tcp_.Connect(&ip, port, [&context]() {
    kprintf("Connected\n");
    event_manager->ActivateContext(context);
  });
  event_manager->SaveContext(context);
}