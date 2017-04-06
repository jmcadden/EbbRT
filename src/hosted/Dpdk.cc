//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include <cstdio>
#include <iostream>
#include "Dpdk.h"

unsigned int portmask;

/*
 * Initilizae a given port using global settings
 */
int
ebbrt::DpdkNetDriver::ConfigurePort(uint8_t port_id)
{
	struct ether_addr addr;
	const uint16_t rxRings = 1, txRings = 1;
	int ret;
	uint16_t q;
  struct rte_eth_conf port_conf_default;
  port_conf_default.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
  port_ = port_id;

	if (port_id > rte_eth_dev_count())
		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));

	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool_ = rte_pktmbuf_pool_create("mbuf_pool", MBUF_PER_POOL,
			MBUF_POOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
			rte_socket_id());
	if (mbuf_pool_ == NULL)
		rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));

	/* Configure the Ethernet device. */
	ret = rte_eth_dev_configure(port_id, rxRings, txRings, &port_conf_default);
	if (ret < 0)
		return ret;

	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rxRings; q++) {
		ret = rte_eth_rx_queue_setup(port_id, q, RX_DESC_PER_QUEUE,
				rte_eth_dev_socket_id(port_id), NULL,
				mbuf_pool_);
		if (ret < 0)
			return ret;
	}

	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < txRings; q++) {
		ret = rte_eth_tx_queue_setup(port_id, q, TX_DESC_PER_QUEUE,
				rte_eth_dev_socket_id(port_id), NULL);
		if (ret < 0)
			return ret;
	}

	/* Start the Ethernet port. */
	ret = rte_eth_dev_start(port_id);
	if (ret < 0)
		return ret;

	/* Display the port MAC address. */
	rte_eth_macaddr_get(port_id, &addr);
  // TODO: mac_addr_ = new ebbrt::EthernetAddress(addr.addr_bytes);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			" %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			(unsigned)port_id,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	rte_eth_promiscuous_enable(port_id);

	return 0;
}

/*
 * Initialize the DPDK Environment Abstraction Layer
 */
int ebbrt::Dpdk::Init(int argc, char** argv) {

 	/* Initialize DPDK Environment Abstraction Layer */
	auto ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

  // Check for Ethernet device
	auto nb_ports = 1;//rte_eth_dev_count();
	if (nb_ports == 0)
		rte_exit(EXIT_FAILURE, "error: no ethernet ports detected\n");
//	if (nb_ports > 1)
//		rte_exit(EXIT_FAILURE, "error: more than one ethernet port found (we only support 1) \n");
//	if (nb_ports != 1 && (nb_ports & 1))
//		rte_exit(EXIT_FAILURE, "Error: number of ports must be even, except "
//				"when using a single port\n");

	/* Initialize the first port available*/
  	auto eth_dev = new DpdkNetDriver();
		if(eth_dev->ConfigurePort(0) != 0 ){
			rte_exit(EXIT_FAILURE, "%s\n", rte_strerror(rte_errno));
		}
		return 0;
}

ebbrt::DpdkNetDriver::DpdkNetDriver()
    : itf_(network_manager->NewInterface(*this)) {
	// Ref to Per-core NetDriverRep 
  ebb_ = DpdkNetRep::Create(this, ebb_allocator->AllocateLocal());
  event_manager->Spawn([=]() { ebb_->Start(); }, true);
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

/* DpdkNetRep */

void ebbrt::DpdkNetRep::Start(){
  std::cout << "Beginning Receive Poll" << std::endl;
	timer->Start(*this, std::chrono::microseconds(POLL_FREQ), true);
}

void ebbrt::DpdkNetRep::Fire(){
	ReceivePoll();
}


void ebbrt::DpdkNetRep::ReceivePoll() {
	/* Get burst of RX packets from the assigned port */
	struct rte_mbuf *bufs[BURST_SIZE];
	const uint16_t nb_rx = rte_eth_rx_burst(eth_dev_.port_, 0,
			bufs, BURST_SIZE);
  if( nb_rx != 0){
    std::cout << "Core "<< rte_lcore_id() <<  " received " << nb_rx << " packets" << std::endl;
    for( int i=0; i<nb_rx; ++i){
      auto p = bufs[i];
      std::cout << "Packet #" << i << " type=" << p->packet_type << " plen=" << p->pkt_len << " dlen=" << p->data_len << " buflen=" << p->buf_len << std::endl;
    }
  }
  // root_.itf_.Receive(std::move(b));
  return;
}

#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
