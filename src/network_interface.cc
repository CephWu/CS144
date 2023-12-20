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
  : ethernet_address_( ethernet_address ), ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  auto dstip = next_hop.ipv4_numeric();
  auto arp_found = _arp_cache.find( dstip );
  if ( arp_found != _arp_cache.end() ) {
    _send_datagram( dgram, arp_found->second.first );
    return;
  }

  auto [it, inserted] = _pending_datagrams.emplace( dstip, decltype( _pending_datagrams )::mapped_type {} );
  auto& [pending_list, time_passed] = it->second;
  // Push datagram into pending datagrams but not drop even timeout (said in FAQ)
  pending_list.emplace_back( dgram );

  bool send_arp_req = inserted;
  if ( !inserted && time_passed >= ARP_REQUEST_COOLDOWN ) {
    // seems impossible to happen in this lab
    time_passed = 0;
    send_arp_req = true;
  }

  if ( send_arp_req ) {
    // ARP probe could avoid risk updating other host's ARP cache
    // but here is forced to be normal
    ARPMessage arp_request {
      .opcode = ARPMessage::OPCODE_REQUEST,
      .sender_ethernet_address = this->ethernet_address_,
      .sender_ip_address = this->ip_address_.ipv4_numeric(),
      .target_ip_address = dstip,
    };
    _send_queue.emplace( EthernetFrame {
      {
        .dst = ETHERNET_BROADCAST,
        .src = this->ethernet_address_,
        .type = EthernetHeader::TYPE_ARP,
      },
      serialize( arp_request ),
    } );
  }
}

inline void NetworkInterface::_send_datagram( const InternetDatagram& dgram, const EthernetAddress& dst )
{
  _send_queue.emplace( EthernetFrame {
    {
      .dst = dst,
      .src = this->ethernet_address_,
      .type = EthernetHeader::TYPE_IPv4,
    },
    serialize( dgram ),
  } );
}

inline void NetworkInterface::_clear_pendings( uint32_t ip, const EthernetAddress& dst )
{
  // Just for removing the follow warning
  // Function 'recv_frame' has cognitive complexity of 29 (threshold 25)
  if ( auto pendings = _pending_datagrams.extract( ip ); !pendings.empty() ) {
    for ( const auto& datagram : pendings.mapped().first ) {
      _send_datagram( datagram, dst );
    }
  }
}

// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  if ( frame.header.dst != ETHERNET_BROADCAST && frame.header.dst != this->ethernet_address_ ) {
    return {};
  }

  if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
    IPv4Datagram datagram {};
    bool success = parse( datagram, frame.payload );
    if ( !success ) {
      return {};
    }
    return datagram;
  }

  if ( frame.header.type == EthernetHeader::TYPE_ARP ) {

    ARPMessage msg {};
    bool success = parse( msg, frame.payload );
    if ( !success ) {
      return {};
    }

    // Update mapping if there is valid SPA/SHA or TPA/THA pair
    if ( msg.sender_ip_address != 0 && msg.sender_ethernet_address != ETHERNET_ZERO ) {
      _arp_cache[msg.sender_ip_address] = { msg.sender_ethernet_address, 0 };
      if ( msg.opcode == ARPMessage::OPCODE_REPLY ) {
        // Push all pending datagrams with related ip address
        _clear_pendings( msg.sender_ip_address, msg.sender_ethernet_address );
      }
    }
    if ( msg.target_ip_address != 0 && msg.target_ethernet_address != ETHERNET_ZERO ) {
      _arp_cache[msg.target_ip_address] = { msg.target_ethernet_address, 0 };
    }

    if ( msg.opcode == ARPMessage::OPCODE_REQUEST && msg.target_ip_address == this->ip_address_.ipv4_numeric()
         && msg.target_ethernet_address == ETHERNET_ZERO && msg.sender_ethernet_address != ETHERNET_ZERO ) {
      ARPMessage arp_response {
        .opcode = ARPMessage::OPCODE_REPLY,
        .sender_ethernet_address = this->ethernet_address_,
        .sender_ip_address = this->ip_address_.ipv4_numeric(),
        .target_ethernet_address = msg.sender_ethernet_address,
        .target_ip_address = msg.sender_ip_address,
      };

      _send_queue.emplace( EthernetFrame {
        {
          .dst = msg.sender_ethernet_address,
          .src = this->ethernet_address_,
          .type = EthernetHeader::TYPE_ARP,
        },
        serialize( arp_response ),
      } );
    }
  }

  return {};
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  for ( auto& it : _pending_datagrams ) {
    it.second.second += ms_since_last_tick;
  }

  for ( auto it = _arp_cache.begin(); it != _arp_cache.end(); ) {
    it->second.second += ms_since_last_tick;
    // map erase during iterating is valid (said in cppref)
    if ( it->second.second >= ARP_CACHE_EXPIRE_TIMEOUT ) {
      it = _arp_cache.erase( it );
    } else {
      it++;
    }
  }
}

optional<EthernetFrame> NetworkInterface::maybe_send()
{
  if ( !_send_queue.empty() ) {
    auto frame = std::move( _send_queue.front() );
    _send_queue.pop();
    return frame;
  }
  return {};
}
