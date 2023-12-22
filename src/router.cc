#include "router.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  if ( !prefix_length ) {
    _default_route = { next_hop, interface_num };
    return;
  }
  // prefix_length ? route_prefix >> ( 32U - prefix_length ) : 0
  // or: & 0x1f
  _table[{ prefix_length, route_prefix >> ( 32U - prefix_length ) }] = { next_hop, interface_num };
}

void Router::route()
{
  for ( auto& intf : interfaces_ ) {
    while ( auto datagram = intf.maybe_receive() ) {
      if ( datagram->header.ttl <= 1 ) {
        continue;
      }
      datagram->header.ttl--;
      datagram->header.compute_checksum();
      const decltype( _default_route )* route = &_default_route;
      for ( const auto& [prefix, target] : _table ) {
        if ( datagram->header.dst >> ( 32U - prefix.first ) == prefix.second ) {
          route = &target;
          break;
        }
      }
      interface( route->second )
        .send_datagram( *datagram, route->first.value_or( Address::from_ipv4_numeric( datagram->header.dst ) ) );
    }
  }
}
