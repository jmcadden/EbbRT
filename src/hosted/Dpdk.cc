//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include <cstdio>
#include "Dpdk.h"

void ebbrt::Dpdk::Init(){
   std::cout << "DPDK Initialized" << std::endl;
}

#endif // __EBBRT_HOSTED_DPDK_DRIVER__
