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
  if ( !checkpoint ) { // maybe just for speed up?
    return raw_value_ - zero_point.raw_value_;
  }
  // get absoluate seqno offset uint32
  uint32_t offset = raw_value_ - zero_point.raw_value_;
  // calculate absoluate seqno uint64 closest to checkpoint
  uint32_t count = checkpoint / ( 1LL << 32 );
  uint32_t ckpt_offset = checkpoint % ( 1LL << 32 );
  if ( max( offset, ckpt_offset ) - min( offset, ckpt_offset ) > ( 1LL << 31 ) ) {
    if ( ckpt_offset > offset ) {
      count += 1;
    } else if ( count ) {
      count -= 1;
    }
  }
  return ( 1UL << 32 ) * count + offset;
}
