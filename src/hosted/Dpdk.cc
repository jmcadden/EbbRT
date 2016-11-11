//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include <cstdio>
//#include <rte_eal.h>
//#include <rte_ethdev.h>
//#include <rte_cycles.h>
//#include <rte_lcore.h>
//#include <rte_mbuf.h>
#include "Dpdk.h"

int ebbrt::Dpdk::Init(int argc, char** argv) {
  /* initialize the DPDK environment */
  std::cout << "DPDK Initialized" << std::endl;
  return 0;
}

ebbrt::DpdkNetDriver::DpdkNetDriver()
    : itf_(network_manager->NewInterface(*this)) {
  std::cout << "DPDK Driver Root Initialized" << std::endl;
}

void ebbrt::DpdkNetDriver::Init() {
  std::cout << "DPDK Driver Initialized" << std::endl;
  auto eth_dev = new DpdkNetDriver();
  // FIXME: one for each lcore?:
  eth_dev->ebb_ = DpdkNetRep::Create(eth_dev, ebb_allocator->AllocateLocal());
}

void ebbrt::DpdkNetDriver::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo){
  EBBRT_UNIMPLEMENTED();
}

const ebbrt::EthernetAddress& ebbrt::DpdkNetDriver::GetMacAddress() {
  return mac_addr_;
}

void ebbrt::DpdkNetRep::ReceivePoll() {
  std::cout << "DPDK ReceivePoll" << std::endl;
  // root_.itf_.Receive(std::move(b));
  return;
}
/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
// static inline int
// port_init(uint8_t port, struct rte_mempool *mbuf_pool)
//{
//	struct rte_eth_conf port_conf = port_conf_default;
//	const uint16_t rx_rings = 1, tx_rings = 1;
//	int retval;
//	uint16_t q;
//
//	if (port >= rte_eth_dev_count())
//		return -1;
//
//	/* Configure the Ethernet device. */
//	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
//	if (retval != 0)
//		return retval;
//
//	/* Allocate and set up 1 RX queue per Ethernet port. */
//	for (q = 0; q < rx_rings; q++) {
//		retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE,
//				rte_eth_dev_socket_id(port), NULL, mbuf_pool);
//		if (retval < 0)
//			return retval;
//	}
//
//	/* Allocate and set up 1 TX queue per Ethernet port. */
//	for (q = 0; q < tx_rings; q++) {
//		retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE,
//				rte_eth_dev_socket_id(port), NULL);
//		if (retval < 0)
//			return retval;
//	}
//
//	/* Start the Ethernet port. */
//	retval = rte_eth_dev_start(port);
//	if (retval < 0)
//		return retval;
//
//	/* Display the port MAC address. */
//	struct ether_addr addr;
//	rte_eth_macaddr_get(port, &addr);
//	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
//			   " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
//			(unsigned)port,
//			addr.addr_bytes[0], addr.addr_bytes[1],
//			addr.addr_bytes[2], addr.addr_bytes[3],
//			addr.addr_bytes[4], addr.addr_bytes[5]);
//
//	/* Enable RX in promiscuous mode for the Ethernet device. */
//	rte_eth_promiscuous_enable(port);
//
//	return 0;
//}
//
// int ebbrt::Dpdk::Init(int argc, char **argv){
//
//	struct rte_mempool *mbuf_pool;
//	unsigned nb_ports;
//	uint8_t portid;
//
//   std::cout << "DPDK Initialized" << std::endl;
//	 ret = rte_eal_init(argc, argv);
//	if (ret < 0)
//		rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");
//
//	/* Check that there is an even number of ports to send/receive on. */
//	nb_ports = rte_eth_dev_count();
//	if (nb_ports < 2 || (nb_ports & 1))
//		rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");
//
//	/* Creates a new mempool in memory to hold the mbufs. */
//	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
//		MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
//
//	if (mbuf_pool == NULL)
//		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
//
//	/* Initialize all ports. */
//	for (portid = 0; portid < nb_ports; portid++)
//		if (port_init(portid, mbuf_pool) != 0)
//			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n",
//					portid);
//
//	if (rte_lcore_count() > 1)
//		printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");
//
//	/* Call lcore_main on the master core only. */
//	lcore_main();
//
//  return ret;
//}

#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
