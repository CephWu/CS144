#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "wrapping_integers.hh"

class TCPReceiver
{
public:
  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  void receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream );

  /* The TCPReceiver sends TCPReceiverMessages back to the TCPSender. */
  TCPReceiverMessage send( const Writer& inbound_stream ) const;
  TCPReceiver():
    isn(0), fin(0),is_isn_set{0}, is_last_sub(0){}
  //* (note): send is like heartbeat events to notice the sender the status
  //* (copy): In your TCP implementation, you’ll use the first unassembled index as the checkpoint

  //? Why SYN consumes the first index? Why not just stream indices?
  //* It is a workaround to distinguish the ACK is either ISN or first byte in stream.
  //* Example: sender: {seqno:10,SYN:true,payload:""}  receiver: {ackno:11,ws:...}
  //* With SYN length: sender: {seqno:11,payload:"a"}  receiver: {ackno:12,ws:...}
  //* Without SYN length: sender: {seqno:10,payload:"a"}  receiver: {ackno:11,ws:...}
  //* I think the main idea is to make the notion clear, and not necessary.
private:
  Wrap32 isn;
  Wrap32 fin;
  bool is_isn_set;
  bool is_last_sub;
};
