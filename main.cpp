#include <iostream>
#include "src/smbios_parser.hpp"

int main() {
  smbios_parser parser;
  try {
    parser.parse();
  } catch (const std::exception& e) {
    std::cerr << "what: " << e.what() << "\n";
  }

  auto info = parser.find<bios_info>();
  if (info) {
    std::cout << info->vendor << '\n';
  }

  for (const auto& structure : parser.structures) {
    if (auto unknown = dynamic_cast<unknown_struct*>(structure)) {
      std::cout << "unknown type, strings:\n";
      for (const auto& str : unknown->to_strings()) {
        std::cout << "\t" << str << "\n";
      }
    }
  }

  return 0;
}
