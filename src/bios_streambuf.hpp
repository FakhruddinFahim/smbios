//
// Created by Fakhr on 11/10/2024.
//

#ifndef SMBIOS_BIOS_STREAMBUF_HPP
#define SMBIOS_BIOS_STREAMBUF_HPP

#include <cstddef>
#include <cstdint>
#include <streambuf>
#include <vector>

class bios_streambuf : public std::streambuf {
  int offset = 0;
  size_t header_start_pos = 0;
public:

  bios_streambuf(uint8_t* beg, uint8_t* end);

  pos_type
  seekoff(std::basic_streambuf<char>::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) override;

  [[nodiscard]] std::size_t pos() const;

  [[nodiscard]] std::size_t size() const;

  void header_start();

  std::string read_string(int header_len);

  std::vector<std::string> read_strings(int header_len);

  void seek_offset();

  uint8_t read_uint8();

  uint16_t read_uint16();

  uint32_t read_uint32();

  uint64_t read_uint64();
};


#endif //SMBIOS_BIOS_STREAMBUF_HPP
