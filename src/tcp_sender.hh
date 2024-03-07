#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <map>
#include <optional>
#include <queue>


class TCPSender
{
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;
  uint16_t _rto_factor { 1 };
  uint64_t _ms_since_last_ticked {}; // cumulative time since last called
  uint64_t _consecutive_retransmissions {};
  std::queue<uint64_t> _send_queue;
  std::map<uint64_t, TCPSenderMessage> _messages; // abs seqno to message
  uint64_t _popped_bytes {};                      // use bytes_popped() to get abs seqno
  bool _started {};                               // mark SYN written
  bool _finished {};                              // mark FIN written
  uint64_t _last_ackno {};
  uint64_t _window_size { 1 }; // as said in FAQ (for back off RTO)

public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( uint64_t initial_RTO_ms, std::optional<Wrap32> fixed_isn );

  /* Push bytes from the outbound stream */
  void push( Reader& outbound_stream );

  /* Send a TCPSenderMessage if needed (or empty optional otherwise) */
  std::optional<TCPSenderMessage> maybe_send();

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage send_empty_message() const;

  /* Receive an act on a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called. */
  void tick( uint64_t ms_since_last_tick );

  /* Accessors for use in testing */
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
};
