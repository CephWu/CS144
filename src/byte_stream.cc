#include "byte_stream.hh"
#include <algorithm>

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), _buffer(), _bytes_buffered(0), _bytes_popped(0), _bytes_pushed(0), _is_error(0), _is_closed(0)
{}


void Writer::push( string data )
{
  // Your code here.
  for (auto& c: data) {
    if(available_capacity() > 0) {
      _buffer.push(c);
      _bytes_pushed++;
    }
  }
}



void Writer::close()
{
  // Your code here.
  _is_closed = true;

}

void Writer::set_error()
{
  // Your code here.
  _is_error = true;
}

bool Writer::is_closed() const
{
  // Your code here.
  return _is_closed;
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

string_view Reader::peek() const
{
  // Your code here.
  return string_view(&_buffer.front(), 1);
}

bool Reader::is_finished() const
{
  // Your code here.
  if (_is_closed && bytes_buffered() == 0) {
    return true;
  }
  return false;
}

bool Reader::has_error() const
{
  // Your code here.
  return _is_error;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  while (len--) {
    _buffer.pop();
    _bytes_popped++;
  }
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
