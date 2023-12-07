#include "byte_stream.hh"
#include <algorithm>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity )
{
  //* for vector
  // _buffer.reserve( capacity );
}

void Writer::push( const string_view& data )
{
  // Your code here.

  //* string impl: ByteStream throughput: 3.95 Gbit/s
  // auto write_num = min( available_capacity(), data.size() );
  // _bytes_pushed += write_num;
  // _buffer += data.substr( 0, write_num );

  //* vector<char> impl: ByteStream throughput: 3.32 Gbit/s
  auto write_num = min( available_capacity(), data.size() );
  _bytes_pushed += write_num;
  // for ( const auto* it = data.begin(); it != data.begin() + static_cast<int64_t>( write_num ); it++ ) {
  //   _buffer.emplace_back( *it );
  // }
  //* ByteStream throughput: 3.90 Gbit/s
  size_t old_size = _buffer.size();
  _buffer.resize( old_size + write_num );
  copy( data.begin(), data.begin() + write_num, _buffer.begin() + static_cast<int64_t>( old_size ) );

  //* deque<char> impl: ByteStream throughput: 0.13 Gbit/s
  // auto write_num = min( available_capacity(), data.size() );
  // _bytes_pushed += write_num;
  // size_t old_size = _buffer.size();
  // _buffer.resize( old_size + write_num );
  // copy( data.begin(), data.begin() + write_num, _buffer.begin() + static_cast<int64_t>( old_size ) );
}

void Writer::close()
{
  // Your code here.
  _closed = true;
}

void Writer::set_error()
{
  // Your code here.
  _error = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return _closed;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - _buffer.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return _bytes_pushed;
}

// static string _view_str;

string_view Reader::peek() const
{
  // Your code here.
  // if ( _buffer.empty() ) {
  //   return {};
  // }
  // string -> char* -> string_view
  // return { &_buffer.front(), 1 };
  //* for string
  // return { _buffer };
  //* for vector
  return string_view { _buffer.data(), _buffer.size() };
  //* for deque
  // _view_str = string { _buffer.begin(), _buffer.end() };
  // return { _view_str };
}

bool Reader::is_finished() const
{
  // Your code here.
  return writer().is_closed() && !bytes_buffered();
}

bool Reader::has_error() const
{
  // Your code here.
  return _error;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  auto pop_num = min( len, _buffer.size() );
  _bytes_popped += pop_num;
  _buffer.erase( _buffer.begin(), _buffer.begin() + static_cast<int64_t>( pop_num ) );
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return _buffer.size();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return _bytes_popped;
}
