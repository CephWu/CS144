#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <map>
#include <optional>
#include <queue>

//* TCPSender 的 handout 有很多细节没讲清楚，这里做点笔记。
//* 根本没讲清为什么要在 window size != 0 时增加累计重发次数并 back off RTO。
//* 原文说在 push 的时候对 window size 0 特殊处理，也没讲清楚，和上一条是关联的。
//* 还有 window size 的概念，也没讲为什么每次 receive 就需要执行无条件更新，因为 ackno + window size -
//* pushed byte count 可以得到精确值，我第一次做的时候是这么写的，但是卡在了 send_extra 一些莫名其妙的测试上，
//* 后来是卡在了 "Repeated ACKs and outdated ACKs are harmless"，
//* 继续写的话应该可以过测试，但是代码太丑了就重写了。
//* 还有 tick 那里，旨在是为了计时第一个发出的包然后重发，但是理论上，只要在
//* tick 时判断一下有没有 outstanding segment 再增加计时，收到 ack new receipt of data 再重置，
//* 就可以保证计时是精确的，事实上这么实现的可以通过 tick 所有测试。
//* 总而言之，handout 没讲明白原理和细节，只讲流程，根本没有发挥空间。

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
