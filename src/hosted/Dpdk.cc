//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include <cstdio>
#include <iostream>
#include "Dpdk.h"

unsigned int portmask;

void mbuf_receive_poll_(){
	const uint8_t nb_ports = rte_eth_dev_count();
	uint8_t port;

	/*
	 * Check that the port is on the same NUMA node as the polling thread
	 * for best performance.
	 */
	for (port = 0; port < nb_ports; port++)
		if (rte_eth_dev_socket_id(port) > 0 &&
				rte_eth_dev_socket_id(port) !=
						(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
					"polling thread.\n\tPerformance will "
					"not be optimal.\n", port);

	    printf("\nCore %u receiving packets. [Ctrl+C to quit]\n",
			rte_lcore_id());

	/* Run until the application is quit or killed. */
	for (;;) {

			/* Get burst of RX packets, from first port of pair. */
			struct rte_mbuf *bufs[BURST_SIZE];
			const uint16_t nb_rx = rte_eth_rx_burst(port, 0,
					bufs, BURST_SIZE);

			if (unlikely(nb_rx == 0))
				continue;

	    printf("\nCore %u received %d packets.\n",
			rte_lcore_id(), nb_rx);

      /* drop all packets */
			//rte_pktmbuf_free(pkt);
	}
}

/*
 * Configure driver for DPDK Ethernet port
 */
void
ebbrt::DpdkNetDriver::ConfigurePort(uint8_t port_id)
{
	struct ether_addr addr;
	const uint16_t rxRings = 1, txRings = 1;
	const uint8_t nb_ports = rte_eth_dev_count();
	int ret;
	uint16_t q;
  struct rte_eth_conf port_conf_default;
  port_conf_default.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;

	if (port_id > nb_ports)
		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));

  /* Initialize memory pool for device */
	mbuf_pool_ = rte_pktmbuf_pool_create("mbuf_pool", MBUF_PER_POOL,
			MBUF_POOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
			rte_socket_id());

	if (mbuf_pool_ == NULL)
		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));

	ret = rte_eth_dev_configure(port_id, rxRings, txRings, &port_conf_default);
	//if (ret != 0)
	//	return ret;

//	/* Create rings for inter core communication */
//	rx_to_workers = rte_ring_create("rx_to_workers", RING_SIZE, rte_socket_id(),
//			RING_F_SP_ENQ);
//	if (rx_to_workers == NULL)
//		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));
//
//	workers_to_tx = rte_ring_create("workers_to_tx", RING_SIZE, rte_socket_id(),
//			RING_F_SC_DEQ);
//	if (workers_to_tx == NULL)
//		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));
//  return ret;

	for (q = 0; q < rxRings; q++) {
		ret = rte_eth_rx_queue_setup(port_id, q, RX_DESC_PER_QUEUE,
				rte_eth_dev_socket_id(port_id), NULL,
				mbuf_pool_);
		//if (ret < 0)
		//	return ret;
	}

	for (q = 0; q < txRings; q++) {
		ret = rte_eth_tx_queue_setup(port_id, q, TX_DESC_PER_QUEUE,
				rte_eth_dev_socket_id(port_id), NULL);
		//if (ret < 0)
		//	return ret;
	}

	ret = rte_eth_dev_start(port_id);
	//if (ret < 0)
	//	return ret;

	rte_eth_macaddr_get(port_id, &addr);
  //mac_addr_ = new EthernetAddress(addr.addr_bytes);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			" %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			(unsigned)port_id,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	rte_eth_promiscuous_enable(port_id);

	(void)ret;
	return;
}

/*
 * Initialize the DPDK Environment 
 *
 */
int ebbrt::Dpdk::Init(int argc, char** argv) {

	auto ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

	auto nb_ports = rte_eth_dev_count();
	if (nb_ports == 0)
		rte_exit(EXIT_FAILURE, "error: no ethernet ports detected\n");
	if (nb_ports > 1)
		rte_exit(EXIT_FAILURE, "error: more than one ethernet port found (we only support 1) \n");
//	if (nb_ports != 1 && (nb_ports & 1))
//		rte_exit(EXIT_FAILURE, "Error: number of ports must be even, except "
//				"when using a single port\n");

	auto nb_ports_available = nb_ports;

	/* initialize all ports */
	for (auto port_id = 0; port_id < nb_ports; port_id++) {
		/* skip ports that are not enabled */
		if ((portmask & (1 << port_id)) == 0) {
			printf("\nSkipping disabled port %d\n", port_id);
			nb_ports_available--;
			continue;
		}
		/* init port */
		printf("Initializing port %u... done\n", (unsigned) port_id);

  std::cout << "DPDK Driver Initialized" << std::endl;

  auto eth_dev = new DpdkNetDriver();
  // FIXME: one for each lcore?:
  eth_dev->ebb_ = DpdkNetRep::Create(eth_dev, ebb_allocator->AllocateLocal());
	}

	if (!nb_ports_available) {
		rte_exit(EXIT_FAILURE,
			"All available ports are disabled. Please set portmask.\n");
	}
return -1;
}

ebbrt::DpdkNetDriver::DpdkNetDriver()
    : itf_(network_manager->NewInterface(*this)) {
  std::cout << "DPDK Driver Root Initialized" << std::endl;
}


void ebbrt::DpdkNetDriver::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo){
  EBBRT_UNIMPLEMENTED();
//			const uint16_t nb_tx = rte_eth_tx_burst(port ^ 1, 0,
//					bufs, nb_rx);
//
//			/* Free any unsent packets. */
//			if (unlikely(nb_tx < nb_rx)) {
//				uint16_t buf;
//				for (buf = nb_tx; buf < nb_rx; buf++)
//					rte_pktmbuf_free(bufs[buf]);
//			}
}

const ebbrt::EthernetAddress& ebbrt::DpdkNetDriver::GetMacAddress() {
  return mac_addr_;
}

void ebbrt::DpdkNetRep::ReceivePoll() {
  std::cout << "DPDK ReceivePoll" << std::endl;
  // root_.itf_.Receive(std::move(b));
  return;
}

#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
