#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <map>
#include <optional>
#include <queue>


class TCPSender
{
private:
  Wrap32 _isn;
  bool _isSent_ISN;
  bool _isSent_FIN;
  uint64_t _initial_RTO_ms;
  int _cur_RTO_ms;               
  bool _isStartTimer;              
  TCPReceiverMessage _receivedMsg; 
  uint64_t _abs_seqno; 
  uint16_t _primitive_window_size;                       
  std::deque<TCPSenderMessage> _outstanding_collections; 
  std::deque<TCPSenderMessage> _ready_collections;     
  uint64_t _outstanding_bytes;                          
  uint64_t _consecutive_retransmissions_nums;           
  

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
