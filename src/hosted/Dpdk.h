//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#define HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include "../MulticoreEbb.h"
#include "../native/Net.h"
#include "../Debug.h"

#include <rte_config.h>
#include <rte_eal.h>
#include <rte_common.h>
#include <rte_errno.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_malloc.h>
#include <rte_mbuf.h>
#include <rte_mempool.h>
#include <rte_ring.h>

#define RX_RING_SIZE 128
#define TX_RING_SIZE 512

#define RX_DESC_PER_QUEUE 128
#define TX_DESC_PER_QUEUE 512

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32
#define MBUF_PER_POOL 65535
#define MBUF_POOL_CACHE_SIZE 250


#define RING_SIZE 16384
#define REORDER_BUFFER_SIZE 8192
#define MBUF_PER_POOL 65535

namespace ebbrt {
namespace Dpdk {
  int Init(int argc, char** argv); 
  void mbuf_receive_poll_();
}  // namespace Dpdk

class DpdkNetRep;
class DpdkNetDriver : public EthernetDevice {
 public:
  explicit DpdkNetDriver();
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) override;
  const EthernetAddress& GetMacAddress() override;
  void ConfigurePort(uint8_t port_id); 

 private:
	unsigned portid;
  void Start();
  EbbRef<DpdkNetRep> ebb_;
  EthernetAddress mac_addr_;
  NetworkManager::Interface& itf_;
  struct rte_mempool *mbuf_pool_; //make unique ptrs
	rte_ring *rx_to_workers;
	rte_ring *workers_to_tx;

  friend class DpdkNetRep;
};

class DpdkNetRep : public MulticoreEbb<DpdkNetRep, DpdkNetDriver> {
  explicit DpdkNetRep(const DpdkNetDriver& root);
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo);
  void Receive();

 private:
  const DpdkNetDriver& eth_dev_;
  void ReceivePoll();
};
}  // namespace ebbrt

#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
#endif  // HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
