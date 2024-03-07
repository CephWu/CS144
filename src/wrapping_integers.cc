#include "wrapping_integers.hh"
#include <algorithm>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return zero_point + n; // n % ( 1LL << 32 )
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  uint32_t offset = this->raw_value_ - zero_point.raw_value_;
  uint32_t count = checkpoint / (1LL << 32);
  uint32_t left_offset = checkpoint % (1LL << 32);
  if (max(offset, left_offset) - min(offset, left_offset) > (1LL << 31)) {
    if (left_offset > offset) {
      count += 1;
    }else if (count) {
      count -= 1;
    }
  }
  return (1UL << 32) * count + offset;
  
}
