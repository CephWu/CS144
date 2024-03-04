#include "reassembler.hh"
#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  // Your code here.
  // 设置最后一个字符索引
  if ( is_last_substring == true )
    end_index = first_index + data.size();
  _assembleBuffer.resize(output.available_capacity(), 0);
  _flagBuffer.resize(output.available_capacity(), 0);
  first_unassembled_index = output.bytes_pushed();
  first_unacceptavle_index = output.bytes_pushed() + output.available_capacity();
  uint64_t startPos, endPos;
  if (!data.empty()) {
    if (data.size() + first_index < first_unassembled_index || first_index >= first_unacceptavle_index) {
      data = "";
    } else {
      startPos = first_index;
      endPos = first_index + data.size() - 1;
      if (first_index < first_unassembled_index) {
        startPos = first_unassembled_index;
      }
      if (first_index + data.size() > first_unacceptavle_index) {
        endPos = first_unacceptavle_index - 1;
      }
      for (uint64_t i = startPos; i <= endPos; i++) {
        _assembleBuffer[i - first_unassembled_index] = data[i - first_index];
        _flagBuffer[i - first_unassembled_index] = true;
      }
    }
  }

  string tmp;
  while (_flagBuffer.front() == true) {
    tmp += _assembleBuffer.front();
    _flagBuffer.pop_front();
    _assembleBuffer.pop_front();
    _flagBuffer.push_back(0);
    _assembleBuffer.push_back(0);
  }
  output.push(tmp);
  if (output.bytes_pushed() == end_index) {
    output.close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return static_cast<uint64_t>(count( _flagBuffer.begin(), _flagBuffer.end(), true ) );
}
