#include "tcp_receiver.hh"
#include "wrapping_integers.hh"
#include <algorithm>
#include <cstdint>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  // Your code here.
  if ( message.SYN ) {
    _isn = Wrap32 { message.seqno };
  }
  if ( _isn ) {
    reassembler.insert( message.seqno.unwrap( _isn.value(), inbound_stream.bytes_pushed() ) - !message.SYN,
                        message.payload,
                        message.FIN,
                        inbound_stream );
  }
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  // Your code here.
  auto ackno = _isn;
  if ( ackno ) {
    ackno.emplace( ackno.value() + 1 + inbound_stream.bytes_pushed() + inbound_stream.is_closed() );
  }
  return {
    .ackno = ackno,
    .window_size = static_cast<uint16_t>( min( inbound_stream.available_capacity(), uint64_t { UINT16_MAX } ) ),
  };
}
