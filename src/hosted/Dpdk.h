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
#include "../Debug.h"
#include "../Timer.h"

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

#define POLL_FREQ 1000 //microseconds

#define UDP_CHECKSUM_OFFSET 0x6
#define TCP_CHECKSUM_OFFSET 0x10

namespace ebbrt {
namespace Dpdk {
  int Init(int argc, char** argv); 
}  

class DpdkNetRep;
  /* ROLES
   * Initialize EAL
   * Allocate mempool to hold membufs
   * Initilize ports:
   *   - Configure eth dev
   *   - Allocate RX and TX queues
   *   - Start the device, enable permis on RX
   *   - Add callbacks to RX and TX (apply to packen on recv, or pre-send)
   *    - callback to covert to IOBuf?
   *    Depending on driver capabilities advertised by rte_eth_dev_info_get(),
   *    the PMD may support hardware offloading feature like checksumming, TCP
   *    segmentation or VLAN insertion.The support of these offload features
   *    implies the addition of dedicated status bit(s) and value field(s)
   *    into the rte_mbuf data structure. 
  */
class DpdkNetDriver : public EthernetDevice {
public:
  explicit DpdkNetDriver();
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo) override;
  const EthernetAddress& GetMacAddress() override;
  int ConfigurePort(uint8_t port_id); 
  EbbRef<DpdkNetRep> ebb_;

 private:
	uint8_t port_;
  EthernetAddress mac_addr_;
  NetworkManager::Interface& itf_;
  struct rte_mempool *mbuf_pool_; //make unique ptrs
	rte_ring *rx_to_workers;
	rte_ring *workers_to_tx;

  friend class DpdkNetRep;
};

  /* ROLES (PMD)
   * Recv input packets from PMD recv API
   * Process each packet up the IO stack
   * Send pending output packets through PMD transmit API
   * Seperate TX queue per-core per-port
   * Each recv port is assigned too and polled by single core
   * 
   */
class DpdkNetRep : public MulticoreEbb<DpdkNetRep, DpdkNetDriver>, public Timer::Hook {
public:
  explicit DpdkNetRep(const DpdkNetDriver& root) : eth_dev_{root}{};
  void Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo);
  void Receive();
  void Start();

 private:
  void Fire() override;
  void ReceivePoll();
  const DpdkNetDriver& eth_dev_;
};

}  // namespace ebbrt
#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
#endif  // HOSTED_SRC_INCLUDE_EBBRT_DPDK_DPDK_H_
