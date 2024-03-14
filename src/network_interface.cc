#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"
#include "ethernet_header.hh"
#include "ipv4_datagram.hh"
#include "parser.hh"

#include <iostream>

using namespace std;

const EthernetAddress ETHERNET_ZERO {};

// ethernet_address: Ethernet (what ARP calls "hardware") address of the interface
// ip_address: IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( const EthernetAddress& ethernet_address, const Address& ip_address )
  : ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
  , _IP_dgrams()
  , _Ethe_frames()
  , _map_IP2Ethe()
  , _arp_time()
  , _MAP_TTL(30000)
  , _ARP_TTL(5000)
{}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  const uint32_t addr_numeric = next_hop.ipv4_numeric();
  if (_map_IP2Ethe.contains(addr_numeric)) {
    EthernetFrame eth_frame;
    eth_frame.header.type = EthernetHeader::TYPE_IPv4;
    eth_frame.header.src = ethernet_address_;
    eth_frame.header.dst = _map_IP2Ethe[addr_numeric].first;
    eth_frame.payload = serialize(dgram);
    _Ethe_frames.push_back(eth_frame);
  } else {
    if (!_arp_time.contains(addr_numeric)) {
      ARPMessage arp_dgram;
      arp_dgram.opcode = ARPMessage::OPCODE_REQUEST;
      arp_dgram.sender_ethernet_address = ethernet_address_;
      arp_dgram.sender_ip_address = ip_address_.ipv4_numeric();
      // 很明显，目的以太网地址不知道，要等ARP应答
      arp_dgram.target_ip_address = addr_numeric;
      EthernetFrame ethe_frame;
      ethe_frame.header.type = EthernetHeader::TYPE_ARP;
      ethe_frame.header.src = ethernet_address_;
      ethe_frame.header.dst = ETHERNET_BROADCAST;
      ethe_frame.payload =serialize(arp_dgram);
      _IP_dgrams[addr_numeric].push_back(dgram);
      _arp_time.emplace(addr_numeric, 0);
      _Ethe_frames.push_back(ethe_frame);
    }
  }
}


// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  if (frame.header.dst != ethernet_address_ && frame.header.dst != ETHERNET_BROADCAST) {
    return nullopt;
  }
  if (frame.header.type == EthernetHeader::TYPE_IPv4) {
    InternetDatagram idgram;
    if (parse(idgram, frame.payload) == true) {
      return idgram;
    }
    return nullopt;
  }
  if (frame.header.type == EthernetHeader::TYPE_ARP) {
    ARPMessage arp_gram;
    if (parse(arp_gram, frame.payload) == true) {
      _map_IP2Ethe.insert({arp_gram.sender_ip_address, {arp_gram.sender_ethernet_address, 0}});
      if (arp_gram.opcode == ARPMessage::OPCODE_REQUEST) {
        if (arp_gram.target_ip_address == ip_address_.ipv4_numeric()) {
          ARPMessage reply_dgram;
          reply_dgram.opcode = ARPMessage::OPCODE_REPLY;
          reply_dgram.sender_ethernet_address = ethernet_address_;
          reply_dgram.sender_ip_address = ip_address_.ipv4_numeric();
          reply_dgram.target_ethernet_address = arp_gram.sender_ethernet_address;
          reply_dgram.target_ip_address = arp_gram.sender_ip_address;
          EthernetFrame reply_frame;
          reply_frame.header.type = EthernetHeader::TYPE_ARP;
          reply_frame.header.src = reply_dgram.sender_ethernet_address;
          reply_frame.header.dst = reply_dgram.target_ethernet_address;
          reply_frame.payload = serialize(reply_dgram);
          _Ethe_frames.push_back(reply_frame);
        }
      } else if (arp_gram.opcode == ARPMessage::OPCODE_REPLY) {
        _map_IP2Ethe.insert({arp_gram.sender_ip_address, {arp_gram.sender_ethernet_address, 0}});
        auto& ip_dgram = _IP_dgrams[arp_gram.sender_ip_address];
        for (auto& dgram: ip_dgram) {
          send_datagram(dgram, Address::from_ipv4_numeric(arp_gram.sender_ip_address));
        }
        _IP_dgrams.erase(arp_gram.sender_ip_address);
      } 
    }
  }
  return nullopt;
}


// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  for (auto it = _map_IP2Ethe.begin(); it != _map_IP2Ethe.end();) {
    it->second.second += ms_since_last_tick;
    if (it->second.second >= _MAP_TTL) {
      //erase返回下一个元素的迭代器
      it = _map_IP2Ethe.erase(it);
    } else {
      it++;
    }
  }

  for (auto it = _arp_time.begin(); it != _arp_time.end();) {
    it->second += ms_since_last_tick;
    if (it->second >= _ARP_TTL) {
      it = _arp_time.erase(it);
    } else {
      it++;
    }
  }
}

optional<EthernetFrame> NetworkInterface::maybe_send()
{
  if (_Ethe_frames.empty() == true) {
    return nullopt;
  }
  EthernetFrame sent_frame = _Ethe_frames.front();
  _Ethe_frames.pop_front();
  return sent_frame;
}
