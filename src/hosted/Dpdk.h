//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#define HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include "../native/Net.h"

namespace ebbrt {
namespace Dpdk {

 void Init();

}  // namespace Dpdk
}  // namespace ebbrt

#endif // __EBBRT_HOSTED_DPDK_DRIVER__
#endif  // HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
