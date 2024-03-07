#include "tcp_receiver.hh"
#include "wrapping_integers.hh"
#include <algorithm>
#include <cstdint>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  // Your code here.
  if (message.SYN == true) {
    isn = Wrap32(message.seqno);
    is_isn_set = true;
    message.seqno = message.seqno + 1;
  }

  if (message.FIN == true) {
    fin = Wrap32(message.seqno + message.payload.size());
    is_last_sub = true;
  } else {
    is_last_sub = false;
  }

  if (is_isn_set) {
    reassembler.insert(message.seqno.unwrap(isn, inbound_stream.bytes_pushed()) - 1,
      message.payload,
      is_last_sub,
      inbound_stream
    );
  }

  


}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  // Your code here.
  Wrap32 ackno = Wrap32::wrap(inbound_stream.bytes_pushed() + 1, isn);
  TCPReceiverMessage tcpReMsg;
  if (is_isn_set) {
    tcpReMsg.ackno = ackno == fin ? ackno + 1 : ackno;
  }
  tcpReMsg.window_size = inbound_stream.available_capacity() > UINT16_MAX ? UINT16_MAX : inbound_stream.available_capacity();
  return tcpReMsg; 
}
