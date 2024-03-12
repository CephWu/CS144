#include "tcp_sender.hh"
#include "tcp_config.hh"
#include "tcp_sender_message.hh"

#include <algorithm>
#include <random>

using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : _isn( fixed_isn.value_or( Wrap32 { random_device()() } ) )
  , _isSent_ISN( 0 )
  , _isSent_FIN( 0 )
  , _initial_RTO_ms( initial_RTO_ms )
  , _cur_RTO_ms( initial_RTO_ms )
  , _isStartTimer( 0 )
  , _receivedMsg()
  , _abs_seqno( 0 )
  , _primitive_window_size( 1 )
  , _outstanding_collections()
  , _ready_collections()
  , _outstanding_bytes( 0 )
  , _consecutive_retransmissions_nums( 0 )
{
  // 在接收方通知TCPSender之前，应该假设接收器的窗口大小是1
  // 在接收方通知TCPSender之前,初始化_seqno为_isn
  _receivedMsg.ackno = _isn;
  _receivedMsg.window_size = 1;
}
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return _outstanding_bytes;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return _consecutive_retransmissions_nums;
}

//要发就直接从ready集合里面拿一个出来发
optional<TCPSenderMessage> TCPSender::maybe_send()
{
  // Your code here.
  if (_ready_collections.size() == 0) {
    return nullopt;
  }
  TCPSenderMessage msg(_ready_collections.front());
  _ready_collections.pop_front();
  //发送时开始计时
  _isStartTimer = true;
  return msg;
}

//将流里的数据截下来push进暂存两个区，需要考虑是否是第一个SYN,是否是FIN
void TCPSender::push( Reader& outbound_stream )
{
  // Your code here.
  while (_outstanding_bytes < _receivedMsg.window_size) {
    TCPSenderMessage msg;
    if (_isSent_ISN == false) {
      _isSent_ISN = true;
      msg.SYN = true;
      msg.seqno = _isn;
    } else {
      msg.seqno = Wrap32::wrap(_abs_seqno, _isn);
    }

    size_t len = min(
      min( static_cast<size_t>( _receivedMsg.window_size - _outstanding_bytes ), TCPConfig::MAX_PAYLOAD_SIZE ),
      static_cast<size_t>( outbound_stream.bytes_buffered() ) );
    
    read(outbound_stream, len, msg.payload);

    if (outbound_stream.is_finished() == true
      && msg.sequence_length() + _outstanding_bytes < _receivedMsg.window_size) {
        if (_isSent_FIN == false) {
          _isSent_FIN = true;
          msg.FIN = true;
        }
    }
    if (msg.sequence_length() == 0) {
      break;
    } else {
      _ready_collections.push_back(msg);
      _outstanding_collections.push_back(msg);
      _outstanding_bytes += msg.sequence_length();
    }
    _abs_seqno += msg.sequence_length();
  }
}

TCPSenderMessage TCPSender::send_empty_message() const
{
  // Your code here.
  TCPSenderMessage msg;
  msg.seqno = Wrap32::wrap(_abs_seqno, _isn);
  return msg;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  _receivedMsg = msg;
  if (_receivedMsg.window_size == 0) {
    _receivedMsg.window_size = 1;
  }
  _primitive_window_size = msg.window_size;
  if (msg.ackno.has_value() == true) {
    if (msg.ackno.value().unwrap(_isn, _abs_seqno) > _abs_seqno) {
      return;
    }
      while (_outstanding_bytes != 0
        && _outstanding_collections.front().seqno.unwrap(_isn, _abs_seqno)
        + _outstanding_collections.front().sequence_length()
        <= msg.ackno.value().unwrap(_isn, _abs_seqno)) {
          _outstanding_bytes -= _outstanding_collections.front().sequence_length();
          _outstanding_collections.pop_front();
          if (_outstanding_bytes == 0) {
            _isStartTimer = false;
          } else {
            _isStartTimer = true;
          }
          _consecutive_retransmissions_nums = 0;
          _cur_RTO_ms = _initial_RTO_ms;
      }
  }
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  if (_isStartTimer == true) {
    _cur_RTO_ms -= ms_since_last_tick;
  }
  if (_cur_RTO_ms <= 0) {
    _ready_collections.push_front(_outstanding_collections.front());
    _consecutive_retransmissions_nums++;
    if (_primitive_window_size > 0) {
      _cur_RTO_ms = pow(2, _consecutive_retransmissions_nums) * _initial_RTO_ms;
    } else {
      _cur_RTO_ms = _initial_RTO_ms;
    }
  }
}
