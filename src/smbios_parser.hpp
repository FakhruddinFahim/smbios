//
// Created by arch on 04/09/2025.
//

#ifndef SMBIOS_SMBIOS_PARSER_HPP
#define SMBIOS_SMBIOS_PARSER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "smbios.hpp"

struct smbios_parser {
  uint32_t version = 0;
  smbios_entry_point* entry_point = nullptr;
  std::vector<smbios*> structures;

  smbios_parser() = default;

  smbios_parser(const smbios_parser& other) = delete;

  smbios_parser& operator=(const smbios_parser& other) = delete;

  smbios_parser(smbios_parser&& other) noexcept;

  smbios_parser& operator=(smbios_parser&& other) noexcept;

  void parse();

  template<typename T> requires std::is_base_of_v<smbios, T>
  const T* find() {
    for (smbios* smbios : structures) {
      if (auto a = dynamic_cast<T*>(smbios)) {
        return a;
      }
    }
    return nullptr;
  }

  [[nodiscard]] std::string sys_product_name() const;

  [[nodiscard]] std::string sys_uuid() const;

  ~smbios_parser();
};


#endif //SMBIOS_SMBIOS_PARSER_HPP