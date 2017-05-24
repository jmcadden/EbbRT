//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef BAREMETAL_SRC_INCLUDE_EBBRT_ACPI_H_
#define BAREMETAL_SRC_INCLUDE_EBBRT_ACPI_H_

#include <cstdint>
#include <vector>

namespace ebbrt {
namespace acpi {
void BootInit();
void Init();
void PowerOff();
std::vector<uint8_t> PciRootScan();
}
}

#endif  // BAREMETAL_SRC_INCLUDE_EBBRT_ACPI_H_
