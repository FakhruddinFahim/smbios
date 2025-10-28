#include <iostream>
#include "src/smbios_parser.hpp"

int main() {
  auto parser = smbios_parser::parse();
  [[maybe_unused]] auto info = parser.find<bios_info>();
  std::cout << info->vendor << '\n';
  for (const auto& structure: parser.structures) {
    if (auto unknown = dynamic_cast<unknown_struct*>(structure)) {
      std::cout << "unknown type, strings:\n";
      for (const auto& str: unknown->to_strings()) {
        std::cout << "\t" << str << "\n";
      }
    }
  }
  return 0;
}
