//
// Created by arch on 04/09/2025.
//

#include "smbios_parser.hpp"
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <fstream>
#ifdef WIN32
#include <windows.h>
#endif

smbios_parser::smbios_parser(smbios_parser&& other) noexcept :
  version(other.version),
  entry_point(other.entry_point),
  structures(std::move(other.structures)) {
}

smbios_parser& smbios_parser::operator=(smbios_parser&& other) noexcept {
  if (this != &other) {
    version = other.version;
    entry_point = other.entry_point;
    structures = std::move(other.structures);
    other.version = 0;
    other.entry_point = nullptr;
    other.structures.clear();
  }
  return *this;
}

constexpr uint32_t make_fourcc(std::string_view s) {
  return s.size() == 4
           ? (static_cast<uint32_t>(s[0]) << 24) |
           (static_cast<uint32_t>(s[1]) << 16) |
           (static_cast<uint32_t>(s[2]) << 8) |
           static_cast<uint32_t>(s[3])
           : 0;
}

void smbios_parser::parse() {
  std::vector<uint8_t> buffer;

#ifdef WIN32
  uint32_t size = GetSystemFirmwareTable(make_fourcc("RSMB"), 0, nullptr, 0);
  buffer.resize(size, 0);
  GetSystemFirmwareTable(make_fourcc("RSMB"), 0, buffer.data(), size);

  bios_streambuf istream(buffer.data(), buffer.data() + buffer.size());

  auto entry_point64 = new smbios_entry_point64{};
  entry_point = entry_point64;
  istream.sbumpc();
  entry_point64->smbios_major_version = istream.sbumpc();
  entry_point64->smbios_minor_version = istream.sbumpc();
  entry_point64->entry_point_revision = istream.sbumpc();
  entry_point64->structure_table_max_size = istream.read_uint32();
  version = entry_point64->smbios_major_version << 8 | entry_point64->smbios_minor_version;
#else
  std::ifstream entry_file("/sys/firmware/dmi/tables/smbios_entry_point", std::ios_base::binary);
  if (entry_file) {
    auto size = std::filesystem::file_size("/sys/firmware/dmi/tables/smbios_entry_point");
    std::vector<uint8_t> entry_buf(size);
    entry_file.read(reinterpret_cast<char*>(entry_buf.data()), static_cast<std::streamsize>(size));
    if (std::string(entry_buf.data(), entry_buf.data() + 4) == "_SM_") {
      bios_streambuf streambuf(entry_buf.data(), entry_buf.data() + entry_buf.size());
      auto entry_point32 = new smbios_entry_point32{};
      entry_point32->read_params(streambuf, nullptr);
      entry_point = entry_point32;
      version = entry_point32->smbios_major_version << 8 | entry_point32->smbios_minor_version;
    } else if (std::string(entry_buf.data(), entry_buf.data() + 5) == "_SM3_") {
      bios_streambuf streambuf(entry_buf.data(), entry_buf.data() + entry_buf.size());
      auto entry_point64 = new smbios_entry_point64{};
      entry_point64->read_params(streambuf, nullptr);
      entry_point = entry_point64;
      version = entry_point64->smbios_major_version << 8 | entry_point64->smbios_minor_version;
    }
    entry_file.close();
  } else {
    throw std::runtime_error("can not open /sys/firmware/dmi/tables/smbios_entry_point");
  }

  std::fstream file("/sys/firmware/dmi/tables/DMI", std::ios_base::in | std::ios_base::binary);
  if (file) {
    auto size = std::filesystem::file_size("/sys/firmware/dmi/tables/DMI");
    buffer.resize(size);
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    file.close();
  } else {
    throw std::runtime_error("can not open /sys/firmware/dmi/tables/DMI\n");
  }

  bios_streambuf istream(buffer.data(), buffer.data() + buffer.size());
#endif

  if (version > static_cast<int32_t>(smbios_version::smbios_3_5)) {
    version = static_cast<int32_t>(smbios_version::smbios_3_5);
  }

  while (istream.pos() < istream.size()) {
    const auto type = static_cast<smbios_type>(istream.sgetc());
    smbios* s = nullptr;
    if (type == bios_info::TYPE) {
      s = new bios_info{};
    } else if (type == system_info::TYPE) {
      s = new system_info{};
    } else if (type == baseboard_info::TYPE) {
      s = new baseboard_info{};
    } else if (type == system_enclosure::TYPE) {
      s = new system_enclosure{};
    } else if (type == processor_info::TYPE) {
      s = new processor_info{};
    } else if (type == cache_info::TYPE) {
      s = new cache_info{};
    } else if (type == port_connector_info::TYPE) {
      s = new port_connector_info{};
    } else if (type == oem_strings::TYPE) {
      s = new oem_strings{};
    } else if (type == system_configuration_options::TYPE) {
      s = new system_configuration_options{};
    } else if (type == physical_memory_array::TYPE) {
      s = new physical_memory_array{};
    } else if (type == system_boot_info::TYPE) {
      s = new system_boot_info{};
    } else if (type == tpm_device::TYPE) {
      s = new tpm_device{};
    } else if (type == memory_error_info::TYPE) {
      s = new memory_error_info{};
    } else if (type == memory_array_mapped_address::TYPE) {
      s = new memory_array_mapped_address{};
    } else if (type == memory_device::TYPE) {
      s = new memory_device{};
    } else if (type == processor_additional_info::TYPE) {
      s = new processor_additional_info{};
    } else if (type == memory_device_mapped_address::TYPE) {
      s = new memory_device_mapped_address{};
    } else if (type == additional_info::TYPE) {
      s = new additional_info{};
    } else if (type == system_slots::TYPE) {
      s = new system_slots{};
    } else if (type == onboard_devices_extended_info::TYPE) {
      s = new onboard_devices_extended_info{};
    } else if (type == firmware_inventory_info::TYPE) {
      s = new firmware_inventory_info{};
    } else if (type == smbios_type::end_of_table) {
      break;
    } else {
      s = new unknown_struct{};
    }

    s->read(istream, this);
    structures.push_back(s);
  }
}

std::string smbios_parser::sys_product_name() const {
  for (const auto& entry : structures) {
    if (const auto info = dynamic_cast<system_info*>(entry)) {
      return info->product_name;
    }
  }
  return {};
}

std::string smbios_parser::sys_uuid() const {
  for (const auto& entry : structures) {
    if (auto sys = dynamic_cast<system_info*>(entry)) {
      std::stringstream ss;
      ss << std::hex << std::setfill('0');
      for (const auto& byte : sys->uuid) {
        ss << std::setw(2) << static_cast<int>(byte);
      }
      return ss.str();
    }
  }
  return "";
}

smbios_parser::~smbios_parser() {
  delete entry_point;
  for (const auto& structure : structures) {
    delete structure;
  }
  structures.clear();
}
