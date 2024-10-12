//
// Created by Fakhr on 11/10/2024.
//

#include "bios_streambuf.hpp"
#include <istream>

bios_streambuf::bios_streambuf(uint8_t* beg, uint8_t* end) {
  setg(reinterpret_cast<char*>(beg), reinterpret_cast<char*>(beg), reinterpret_cast<char*>(end));
}

bios_streambuf::pos_type
bios_streambuf::seekoff(std::basic_streambuf<char>::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) {
  if (dir == std::ios_base::cur) {
    gbump((int) off);
  } else if (dir == std::ios_base::beg) {
    setg(eback(), eback() + off, egptr());
  } else if (dir == std::ios_base::end) {
    setg(eback(), gptr(), egptr() + off);
  }
  return gptr() - eback();
}


std::size_t bios_streambuf::pos() {
  return gptr() - eback();
}

std::size_t bios_streambuf::size() {
  return egptr() - eback();
}

void bios_streambuf::header_start() {
  header_start_pos = pos();
}

std::string bios_streambuf::read_string(int header_len) {
  int index = sbumpc();
  auto position = pos();
  pubseekoff((int32_t) header_start_pos + header_len, std::ios_base::beg);
  std::string str;
  while (index--) {
    std::getline(std::istream(this), str, '\0');
  }
  pubseekoff((int32_t) position, std::ios_base::beg);
  offset += str.empty() ? 0 : str.size() + 1;
  return str;
}

std::vector<std::string> bios_streambuf::read_strings(int header_len) {
  std::vector<std::string> ret;
  int index = sbumpc();
  if (index <= 0) {
    return ret;
  }
  auto position = pos();
  pubseekoff((int32_t) header_start_pos + header_len, std::ios_base::beg);
  std::string str;
  while (index--) {
    std::getline(std::istream(this), str, '\0');
    ret.emplace_back(str);
    offset += str.size() + 1;
  }
  pubseekoff((int32_t) position, std::ios_base::beg);
  return ret;
}

void bios_streambuf::seek_offset() {
  if (offset > 0) {
    gbump(offset - 1);
  }
  offset = 0;
}

uint8_t bios_streambuf::read_uint8() {
  return sbumpc();
}

uint16_t bios_streambuf::read_uint16() {
  return sbumpc() | sbumpc() << 8;
}

uint32_t bios_streambuf::read_uint32() {
  return static_cast<uint32_t>(sbumpc()) |
         static_cast<uint32_t>(sbumpc() << 8) |
         static_cast<uint32_t>(sbumpc() << 16) |
         static_cast<uint32_t>(sbumpc() << 24);
}

uint64_t bios_streambuf::read_uint64() {
  return (static_cast<uint64_t>(sbumpc()) << 0) |
         (static_cast<uint64_t>(sbumpc()) << 8) |
         (static_cast<uint64_t>(sbumpc()) << 16) |
         (static_cast<uint64_t>(sbumpc()) << 24) |
         (static_cast<uint64_t>(sbumpc()) << 32) |
         (static_cast<uint64_t>(sbumpc()) << 40) |
         (static_cast<uint64_t>(sbumpc()) << 48) |
         (static_cast<uint64_t>(sbumpc()) << 56);
}
