#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "wrapping_integers.hh"

class TCPReceiver
{
private:
  std::optional<Wrap32> _isn {};
  // std::uniform_int_distribution<uint32_t> _gen_isn { 0, UINT32_MAX };
  // std::default_random_engine _rd = get_random_engine();

public:
  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  void receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream );

  /* The TCPReceiver sends TCPReceiverMessages back to the TCPSender. */
  TCPReceiverMessage send( const Writer& inbound_stream ) const;

  //* (note): send is like heartbeat events to notice the sender the status
  //* (copy): In your TCP implementation, you’ll use the first unassembled index as the checkpoint

  //? Why SYN consumes the first index? Why not just stream indices?
  //* Maybe just for the sender to know that receiver had received the ISN?
  //* Example: sender: {seqno:10,SYN:true,payload:""}  receiver: {ackno:11,ws:...}
};
