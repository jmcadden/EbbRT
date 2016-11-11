//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#define HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

//#include <rte_eal.h>
#include "../MulticoreEbb.h"
#include "../native/Net.h"

namespace ebbrt {
namespace Dpdk {
int Init(int argc, char** argv); 
}  // namespace Dpdk
class DpdkNetRep;
class DpdkNetDriver : public EthernetDevice {
 public:
  static void Init();

  explicit DpdkNetDriver();
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) override;
  const EthernetAddress& GetMacAddress() override;

 private:
  void Start();
  EbbRef<DpdkNetRep> ebb_;
  EthernetAddress mac_addr_;
  NetworkManager::Interface& itf_;

  friend class DpdkNetRep;
};

class DpdkNetRep : public MulticoreEbb<DpdkNetRep, DpdkNetDriver> {
  explicit DpdkNetRep(const DpdkNetDriver& root);
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo);
  void Receive();

 private:
  const DpdkNetDriver& root_;
  void ReceivePoll();
};
}  // namespace ebbrt

#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
#endif  // HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
