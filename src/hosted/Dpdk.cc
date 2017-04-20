//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifdef __EBBRT_HOSTED_DPDK_DRIVER__

#include <cstdio>
#include <iostream>
#include "Dpdk.h"
#include "../Timer.h"
#include "../MulticoreEbb.h"
#include "../UniqueIOBuf.h"
#include "../StaticIOBuf.h"
#include "../IOBufRef.h"
#include "../Debug.h"

#include <rte_ip.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <rte_ethdev.h>

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
  mac_addr_[0]=addr.addr_bytes[0];
  mac_addr_[1]=addr.addr_bytes[1];
  mac_addr_[2]=addr.addr_bytes[2];
  mac_addr_[3]=addr.addr_bytes[3];
  mac_addr_[4]=addr.addr_bytes[4];
  mac_addr_[5]=addr.addr_bytes[5];
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
			" %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
			(unsigned)port_id,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	/* Enable RX in promiscuous mode for the Ethernet device. */
	rte_eth_promiscuous_enable(port_id);

  //TODO: Verify checksum offload
#if 0
  struct rte_eth_dev_info info;
  rte_eth_dev_info_get(eth_dev_.port_, &info);
  bool support = info.tx_offload_capa & DEV_TX_OFFLOAD_UDP_CKSUM;
  ebbrt::kbugon(!support);
#endif

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
  event_manager->Spawn([=]() { ebb_->Start(); 
      
          std::cout << "Starting Dhcp " << std::endl;
        network_manager->StartDhcp().Then([](Future<void> fut) {
          fut.Get();
          std::cout << "Dhcp Complete" << std::endl;
        });

      }, true);
}

void ebbrt::DpdkNetDriver::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo){
  ebb_->Send(std::move(buf), std::move(pinfo));
}

/*
struct PacketInfo {
  static const constexpr uint8_t kNeedsCsum = 1;
  static const constexpr uint8_t kGsoNone = 0;
  static const constexpr uint8_t kGsoTcpv4 = 1;
  static const constexpr uint8_t kGsoUdp = 3;
  static const constexpr uint8_t kGsoTcpv6 = 4;

  uint8_t flags{0};
  uint8_t gso_type{0};
  uint16_t hdr_len{0};
  uint16_t gso_size{0};
  uint16_t csum_start{0};
  uint16_t csum_offset{0};
};
*/

void ebbrt::DpdkNetRep::Send(std::unique_ptr<IOBuf> buf, PacketInfo pinfo){
  struct rte_mbuf* mbuf;

  if(buf->Capacity() >= sizeof(struct rte_mbuf)){
    // Check if the IOBuf contains a mbuf
    auto bbuf = const_cast<uint8_t*>(buf->Buffer());
    mbuf = reinterpret_cast<struct rte_mbuf*>(bbuf);
    if(mbuf->buf_addr == bbuf){
      std::cout << "Ok, so we already have an mbuf!" << std::endl;
      EBBRT_UNIMPLEMENTED();
    }
  }

  // XXX: Copy IOBuf into a new mbuf
  auto buf_len = buf->ComputeChainDataLength();
  mbuf = rte_pktmbuf_alloc(eth_dev_.mbuf_pool_);
  mbuf->ol_flags = 0; 
  mbuf->data_len = buf_len;
  mbuf->pkt_len = buf_len;
  auto mbuf_data = rte_pktmbuf_mtod(mbuf, uint8_t *);
  for (auto& b : *buf) {
      auto len = b.Length();
      std::memcpy(static_cast<void*>(mbuf_data), b.Data(), len);
      mbuf_data += len;
  }

  /* Segmentation offload */
  ebbrt::kbugon(pinfo.gso_type != PacketInfo::kGsoNone, "DPDK drives do not YET support segment offloading"); 

  /* Checksum offload */
  if (pinfo.flags & PacketInfo::kNeedsCsum) {
    switch(pinfo.csum_offset){
      case UDP_CHECKSUM_OFFSET: 
        mbuf->ol_flags |= PKT_TX_UDP_CKSUM;
        break;
      case TCP_CHECKSUM_OFFSET:
        mbuf->ol_flags |= PKT_TX_TCP_CKSUM;
        break;
      default:
        ebbrt::kabort("Unknown checksum type");
    }
    mbuf->ol_flags |= PKT_TX_IPV4;
    mbuf->l2_len = sizeof(struct ether_hdr);
    mbuf->l3_len = sizeof(struct ipv4_hdr);
    std::cout << "Blah" << std::endl;
  }else{
    mbuf->ol_flags |= PKT_TX_L4_NO_CKSUM;
  }

#if 0
  // TODO: UDP/TCP Checksum Offloading
  /* Verify Checksum */
  auto ip_hdr =  (struct ipv4_hdr*)(rte_pktmbuf_mtod(mbuf, uint8_t*) + mbuf->l2_len); 
  auto udp_hdr =(struct udp_hdr*)(rte_pktmbuf_mtod(mbuf, uint8_t*) + mbuf->l2_len + mbuf->l3_len);
  //std::cout << "OFFSET: Our= " << pinfo.csum_start + pinfo.csum_offset  << std::endl;
  //std::cout << "OFFSET: Thier= " << mbuf->l2_len + mbuf->l3_len  << std::endl;
  std::cout << "ORIGINAL PCKSUM " << udp_hdr->dgram_cksum << std::endl;
  auto save1 = udp_hdr->dgram_cksum;
  auto save2 = ip_hdr->hdr_checksum;
  ip_hdr->hdr_checksum = 0;
  udp_hdr->dgram_cksum = 0;
  std::cout << "IPV4 PCKSUM: "<<  rte_ipv4_phdr_cksum(ip_hdr,0) << std::endl;
  //std::cout << "UDP CKSUM: "<<  rte_ipv4_udptcp_cksum(ip_hdr,udp_hdr) << std::endl;
  ip_hdr->hdr_checksum = save2;
  udp_hdr->dgram_cksum = save1;
  
      ebbrt::kabort("BYE ");
#endif

  /* Send packet */
	auto nb_tx = rte_eth_tx_burst(eth_dev_.port_, 0, &mbuf, 1);
  std::cout << nb_tx << " packets sent!" << std::endl;
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
      std::cout << "Packet #" << i 
        << " type=" << p->packet_type 
        << " pkt len=" << rte_pktmbuf_pkt_len(p) // sum of all segments
        << " data len=" << rte_pktmbuf_data_len(p) // sum in segment buffer
        << " buflen=" << p->buf_len 
        << " segments=" << p->nb_segs 
        << std::endl;

      /* check for multi-segment mbufs */
      ebbrt::kbugon((rte_pktmbuf_pkt_len(p) != rte_pktmbuf_data_len(p)));
      /* check for control mbufs */
      ebbrt::kbugon(rte_is_ctrlmbuf(p));


      ///* Construct IOBuf containing the full mbuf */
      // auto rbuf = MakeUniqueIOBuf(rte_pktmbuf_data_len(p));
      // auto dp = rbuf->GetMutDataPointer();       
      // auto mbuf_data = rte_pktmbuf_mtod(p, uint8_t *);
      // memcpy(dp.Data(), mbuf_data, rte_pktmbuf_data_len(p));
      std::unique_ptr<DpdkIOBuf> rbuf = DpdkIOBuf::Create<DpdkIOBuf>(p);

      
      eth_dev_.itf_.Receive(std::move(rbuf));
    }
  }
  return;
}

/*
 * DpdkIOBufOwner
 */
ebbrt::DpdkIOBufOwner::DpdkIOBufOwner(struct rte_mbuf* mbuf) : mbuf_((const struct rte_mbuf*)mbuf) {
  if(!rte_pktmbuf_is_contiguous(mbuf)) {
    ebbrt::kabort("// TODO: segmented mbuf support"); 
  }
  buffer_ = rte_pktmbuf_mtod(mbuf, const uint8_t*);
  capacity_ = static_cast<size_t>(rte_pktmbuf_data_len(mbuf_));
} 

ebbrt::DpdkIOBufOwner::~DpdkIOBufOwner(){
  rte_pktmbuf_free(const_cast<struct rte_mbuf*>(mbuf_)); 
}


#endif  // __EBBRT_HOSTED_DPDK_DRIVER__
