#pragma once

#include "byte_stream.hh"

#include <map>
#include <string>

class Reassembler
{
public:
  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  //? my note here about the algorithm detail
  /*
   * "should discard any bytes that lie beyond the stream's available capacity"?
   * "The Reassembler’s will not store any bytes that can’t be pushed to the ByteStream either immediately"?
   * last pushed string should be truncated if not fit capacity.
   *
   * In abstract, all incoming substrings of the "underlying byte stream" are in random order,
   * random index and random length. The goal is to reassemble them.
   *
   * example("abcdefgh") cap=8 [push "bcd",1] [push "cde",2] [push "g",6] [push "hijk",7]
   * unassembled strings will be {1:"bcd",2:"cde",6:"g",7:"h"}
   * 1. "bcd" and "cde" overlapping should be resolved to "bcde".
   * 2. when received [push "a",0] (even [push "abc",0]), reassember should push all push-able string
   * (this case is "abcde") and wait for next substring ("f").
   * 3. "hijk" truncated to "h".
   * 4. when do [push "cdef", 2], it shoule replace the "cde" to the longer "cdef" on index 2.
   * as the example unassembled strings above. if bytes_pushed=2 (pushed "ab"), then reassembler
   * should be able to find "bcd" and push "cd" (such as remember the last bytes_pushed)
   *
   * 目前的实现有些混乱，因为我一开始误以为传入的子串 index 都是提前分割好固定的，也误以为 overlapping 只是针对
   * 同 index 而言，花了很多时间去做 test fit。最后才知道是完全随机的。
   */

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

private:
  //* Reassembler throughput: 12.20 Gbit/s
  std::map<size_t, std::string> _unassembled_strings;
  size_t _last_string_end;
  size_t _last_popped_end;
  void _push_availables( Writer& output, size_t index );
};
