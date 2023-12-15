#include "tcp_sender.hh"
#include "tcp_config.hh"
#include "tcp_sender_message.hh"

#include <algorithm>
#include <random>

using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms )
{}

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  //* Get the length of all segments not acked (no bigger than window size)
  // TODO(iyume): Optimize
  uint64_t res {};
  for ( const auto& it : _messages ) {
    res += it.second.sequence_length();
  }
  return res;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return _consecutive_retransmissions;
}

optional<TCPSenderMessage> TCPSender::maybe_send()
{
  // Your code here.
  if ( !_send_queue.empty() ) {
    uint64_t seqno = _send_queue.front();
    _send_queue.pop();
    // Retransmittion is possibly not in the flighting messages
    if ( const auto& msg = _messages.find( seqno ); msg != _messages.end() ) {
      return msg->second;
    }
  }
  return {};
}

void TCPSender::push( Reader& outbound_stream )
{
  // Your code here.
  //* This function read stream into segments as many as possible
  //* The sequence number in flight should not beyond the window size
  //* Specially, pretend window size as one only if no sequence number in flight and no window size

  using value_type = decltype( _messages )::value_type;
  // New definition is needed because window_size enables RTO back off
  uint16_t window_size = _window_size;
  if ( !window_size && !sequence_numbers_in_flight() ) {
    window_size = 1;
  }

  if ( _finished ) {
    return;
  }

  string payload;

  while ( window_size > sequence_numbers_in_flight() ) {
    bool syn {};
    bool fin {};
    uint64_t abs_seqno = 1 + outbound_stream.bytes_popped();
    uint16_t read_size = window_size - sequence_numbers_in_flight();

    if ( !_started ) {
      _started = true;
      syn = true;
      read_size -= 1;
      abs_seqno -= 1;
    }
    // TEST: MAX_PAYLOAD_SIZE limits payload only
    uint16_t payload_max_size = min( read_size, static_cast<uint16_t>( TCPConfig::MAX_PAYLOAD_SIZE ) );
    read( outbound_stream, payload_max_size, payload );
    read_size -= payload.size();
    if ( !_finished && outbound_stream.is_finished() && read_size ) {
      _finished = true;
      fin = true;
    }
    if ( !( syn + payload.size() + fin ) ) {
      break; // nothing to send
    }

    _messages.emplace( value_type { abs_seqno, { isn_ + abs_seqno, syn, { payload }, fin } } );
    _send_queue.emplace( abs_seqno );
  }

  _popped_bytes = outbound_stream.bytes_popped();
}

TCPSenderMessage TCPSender::send_empty_message() const
{
  // Your code here.
  return { .seqno = isn_ + _started + _popped_bytes + _finished };
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  //* In the tests, the ACK triggers push method to call
  //* TEST "SYN + FIN", is it possible to receive msg without ackno and take new window size?

  _window_size = msg.window_size; // update unconditional

  if ( msg.ackno ) {
    // just find the closest ackno to last ackno, but not to the stream's end (maybe more accurate)
    uint64_t ackno = msg.ackno->unwrap( isn_, _last_ackno );
    if ( ( _last_ackno && ackno <= _last_ackno ) || ackno > _started + _popped_bytes + _finished ) {
      // (_last_ackno && ackno <= _last_ackno) is a trick to let timer not restart
      // because _last_ackno is not initialized less than 0
      return;
    }
    _last_ackno = ackno;

    //? try some lower_bound trick?
    // _messages.erase( _messages.begin(), --_messages.lower_bound( ackno ) );
    auto erase_end = _messages.begin();
    while ( erase_end != _messages.end() ) {
      if ( ackno < erase_end->first + erase_end->second.sequence_length() ) {
        break;
      }
      erase_end++;
    }
    _messages.erase( _messages.begin(), erase_end );

    _rto_factor = 1;
    _consecutive_retransmissions = 0;
    _ms_since_last_ticked = 0;
  }
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  if ( _messages.empty() ) {
    return;
  }
  _ms_since_last_ticked += ms_since_last_tick;
  if ( _ms_since_last_ticked >= initial_RTO_ms_ * _rto_factor ) {
    _send_queue.emplace( _messages.begin()->first );
    if ( _window_size ) {
      //* Back off RTO only if the network is lousy.
      //* The zero window size indicates that receiver is busy, and there is only one special
      //* message in the flighting messages which only contains one length.
      _rto_factor *= 2;
      _consecutive_retransmissions += 1;
    }
    _ms_since_last_ticked = 0;
    return;
  }
}
