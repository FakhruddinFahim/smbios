#include <iostream>
#include "src/smbios.hpp"

int main() {
  auto ctx = parse_smbios();
  for (const auto& structure: ctx.structures) {
    if (auto unknown = dynamic_cast<unknown_struct*>(structure)) {
      std::cout << "unknown type, strings:\n";
      for (const auto& str: unknown->to_strings()) {
        std::cout << "\t" << str << "\n";
      }
    }
  }
  return 0;
}
