//
// Created by Fakhr on 24/05/2024.
//

#include "smbios.hpp"
#include <cstring>
#include <format>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef WIN32

#include <windows.h>

#else
#include <filesystem>
#include <fstream>
#endif

void smbios_header::read(bios_streambuf& istream) {
  type = istream.sbumpc();
  length = istream.sbumpc();
  handle = istream.sbumpc() | istream.sbumpc() << 8;
}

void smbios_entry_point32::read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) {
  istream.sgetn(reinterpret_cast<char*>(anchor_string), sizeof(anchor_string));
  entry_point_structure_checksum = istream.sbumpc();
  entry_point_length = istream.sbumpc();
  smbios_major_version = istream.sbumpc();
  smbios_minor_version = istream.sbumpc();
  max_structure_size = istream.sbumpc();
  entry_point_revision = istream.sbumpc();
  istream.sgetn(reinterpret_cast<char*>(formatted_area), sizeof(formatted_area));
  istream.sgetn(reinterpret_cast<char*>(intermediate_anchor_string), sizeof(intermediate_anchor_string));
  intermediate_checksum = istream.sbumpc();
  structure_table_length = istream.read_uint16();
  structure_table_addr = istream.read_uint32();
  num_of_smbios_structures = istream.read_uint16();
  smbios_bcd_revision = istream.sbumpc();
}

void smbios_entry_point64::read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) {
  istream.sgetn(reinterpret_cast<char*>(anchor_string), sizeof(anchor_string));
  entry_point_structure_checksum = istream.sbumpc();
  entry_point_length = istream.sbumpc();
  smbios_major_version = istream.sbumpc();
  smbios_minor_version = istream.sbumpc();
  smbios_docrev = istream.sbumpc();
  entry_point_revision = istream.sbumpc();
  reserved = istream.sbumpc();
  structure_table_max_size = istream.read_uint32();
  structure_table_addr = istream.read_uint64();
}

void smbios_ctx::parse(std::vector<uint8_t>& buf) {
  this->buffer = buf;
  bios_streambuf istream(buffer.data(), buffer.data() + buffer.size());
#ifdef WIN32
  auto entry_point64 = new smbios_entry_point64{};
  entry_point = entry_point64;
  istream.sbumpc();
  entry_point64->smbios_major_version = istream.sbumpc();
  entry_point64->smbios_minor_version = istream.sbumpc();
  entry_point64->entry_point_revision = istream.sbumpc();
  entry_point64->structure_table_max_size = istream.read_uint32();
  version = entry_point64->smbios_major_version << 8 | entry_point64->smbios_minor_version;
#else
  auto size = std::filesystem::file_size("/sys/firmware/dmi/tables/smbios_entry_point");
  std::ifstream file("/sys/firmware/dmi/tables/smbios_entry_point", std::ios_base::binary);
  if (file) {
    std::vector<uint8_t> entry_buf(size);
    file.read(reinterpret_cast<char *>(entry_buf.data()), static_cast<std::streamsize>(size));
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
  } else {
    throw std::runtime_error("can not read smbios_entry_point");
  }

#endif
  if (version > static_cast<int32_t>(smbios_version::SMBIOS_3_5)) {
    version = static_cast<int32_t>(smbios_version::SMBIOS_3_5);
  }


  while (istream.pos() < istream.size()) {
    const uint8_t type = istream.sgetc();
    smbios* s = nullptr;
    if (type == bios_info::TYPE) {
      s = new bios_info{};
    } else if (type == sys_info::TYPE) {
      s = new sys_info{};
    } else if (type == baseboard_info::TYPE) {
      s = new baseboard_info{};
    } else if (type == sys_enclosure::TYPE) {
      s = new sys_enclosure{};
    } else if (type == processor_info::TYPE) {
      s = new processor_info{};
    } else if (type == cache_info::TYPE) {
      s = new cache_info{};
    } else if (type == port_connector_info::TYPE) {
      s = new port_connector_info{};
    } else if (type == oem_strings::TYPE) {
      s = new oem_strings{};
    } else if (type == sys_configuration_options::TYPE) {
      s = new sys_configuration_options{};
    } else if (type == physical_memory_arr::TYPE) {
      s = new physical_memory_arr{};
    } else if (type == sys_boot_info::TYPE) {
      s = new sys_boot_info{};
    } else if (type == tpm_device::TYPE) {
      s = new tpm_device{};
    } else if (type == memory_err_info::TYPE) {
      s = new memory_err_info{};
    } else if (type == memory_arr_mapped_addr::TYPE) {
      s = new memory_arr_mapped_addr{};
    } else if (type == memory_device::TYPE) {
      s = new memory_device{};
    } else if (type == processor_additional_info::TYPE) {
      s = new processor_additional_info{};
    } else if (type == memory_device_mapped_address::TYPE) {
      s = new memory_device_mapped_address{};
    } else if (type == additional_info::TYPE) {
      s = new additional_info{};
    } else if (type == system_slot::TYPE) {
      s = new system_slot{};
    } else if (type == onboard_devices_extended_info::TYPE) {
      s = new onboard_devices_extended_info{};
    } else if (type == firmware_inventory_info::TYPE) {
      s = new firmware_inventory_info{};
    } else if (type == 127) {
      break;
    } else {
      s = new unknown_struct{};
    }

    s->read(istream, this);
    structures.push_back(s);
  }
}

std::string smbios_ctx::sys_product_name() {
  for (const auto& entry: structures) {
    if (const auto info = dynamic_cast<sys_info*>(entry)) {
      return info->product_name;
    }
  }
  return {};
}

std::string smbios_ctx::sys_uuid() {
  for (const auto& entry: structures) {
    if (auto sys = dynamic_cast<sys_info*>(entry)) {
      std::stringstream ss;
      ss << std::hex << std::setfill('0');
      for (const auto& byte: sys->uuid) {
        ss << std::setw(2) << static_cast<int>(byte);
      }
      return ss.str();
    }
  }
  return "";
}

smbios_ctx::~smbios_ctx() {
  for (const auto& structure: structures) {
    delete structure;
  }
  structures.clear();
}

void smbios::read(bios_streambuf& istream, smbios_ctx* smbios_ctx) {
  istream.header_start();
  header.read(istream);
  read_params(istream, smbios_ctx);
  istream.seek_offset();
  uint8_t a = istream.sbumpc();
  if (a == 0 && istream.sgetc() == 0) {
    istream.sbumpc();
  } else {
    throw std::runtime_error(std::format("structure terminator not found, type: {}", header.type));
  }
}

smbios_ctx parse_smbios() {
  std::vector<uint8_t> buffer;
#ifdef WIN32
  uint32_t size = GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
  buffer.resize(size, 0);
  GetSystemFirmwareTable('RSMB', 0, buffer.data(), size);
#else
  std::error_code ec;
  auto size = std::filesystem::file_size("/sys/firmware/dmi/tables/DMI", ec);
  std::fstream file("/sys/firmware/dmi/tables/DMI", std::ios_base::in | std::ios_base::binary);
  if (file.is_open()) {
    buffer.resize(size);
    file.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    file.close();
  } else {
    std::print(std::cerr, "error reading file\n");
  }
#endif

  smbios_ctx smbios{};
  smbios.parse(buffer);
  return smbios;
}

void bios_info::read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) {
  if (smbios_ctx->version >= smbios_version::SMBIOS_2_0) {
    vendor = istream.read_string(header.length);
    bios_version = istream.read_string(header.length);
    bios_starting_addr_seg = istream.read_uint16();
    bios_release_date = istream.read_string(header.length);
    bios_rom_size = istream.sbumpc();
    bios_characteristics = istream.read_uint64();
  }
  if (smbios_ctx->version == smbios_version::SMBIOS_2_1 || smbios_ctx->version == smbios_version::SMBIOS_2_2) {
    bios_characteristics_ext_bytes.push_back(istream.sbumpc());
  } else if (smbios_ctx->version >= smbios_version::SMBIOS_2_3) {
    bios_characteristics_ext_bytes.push_back(istream.sbumpc());
    bios_characteristics_ext_bytes.push_back(istream.sbumpc());
  }
  if (smbios_ctx->version >= smbios_version::SMBIOS_2_4) {
    system_bios_major_release = istream.sbumpc();
    system_bios_minor_release = istream.sbumpc();
    embedded_controller_firmware_major_release = istream.sbumpc();
    embedded_controller_firmware_minor_release = istream.sbumpc();
  }
  if (smbios_ctx->version >= smbios_version::SMBIOS_3_1) {
    extended_bios_rom_size = istream.read_uint16();
  }
}

void sys_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_0) {
    manufacturer = istream.read_string(header.length);
    product_name = istream.read_string(header.length);
    version = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
  }
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    istream.sgetn(reinterpret_cast<char*>(uuid), sizeof(uuid));
    wake_up_type = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_4) {
    sku_number = istream.read_string(header.length);
    family = istream.read_string(header.length);
  }
}

void baseboard_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  manufacturer = istream.read_string(header.length);
  product = istream.read_string(header.length);
  version = istream.read_string(header.length);
  serial_number = istream.read_string(header.length);
  asset_tag = istream.read_string(header.length);
  feature_flags = istream.sbumpc();
  location_in_chassis = istream.read_string(header.length);
  chassis_handle = istream.read_uint16();
  board_type = istream.sbumpc();
  num_of_contained_object_handles = istream.sbumpc();
  for (int i = 0; i < num_of_contained_object_handles; ++i) {
    contained_object_handles.push_back(istream.read_uint16());
  }
}

void sys_enclosure::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_0) {
    manufacturer = istream.read_string(header.length);
    type = istream.sbumpc();
    version = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
    asset_tag_number = istream.read_string(header.length);
  }
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    boot_up_state = istream.sbumpc();
    power_supply_state = istream.sbumpc();
    thermal_state = istream.sbumpc();
    security_status = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_3) {
    oem_defined = istream.read_uint32();
    height = istream.sbumpc();
    num_of_power_cords = istream.sbumpc();
    contained_element_count = istream.sbumpc();
    contained_element_record_length = istream.sbumpc();
    for (int i = 0; i < contained_element_count * contained_element_record_length; ++i) {
      contained_elements.push_back(istream.sbumpc());
    }
  }
  if (smbios->version >= smbios_version::SMBIOS_2_7) {
    sku_number = istream.read_string(header.length);
  }
}

void processor_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_0) {
    socket_designation = istream.read_string(header.length);
    processor_type = istream.sbumpc();
    processor_family = istream.sbumpc();
    processor_manufacturer = istream.read_string(header.length);
    processor_id = istream.read_uint64();
    processor_version = istream.read_string(header.length);
    voltage = istream.sbumpc();
    external_clock = istream.read_uint16();
    max_speed = istream.read_uint16();
    current_speed = istream.read_uint16();
    status = istream.sbumpc();
    processor_upgrade = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    l1_cache_handle = istream.read_uint16();
    l2_cache_handle = istream.read_uint16();
    l3_cache_handle = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_3) {
    serial_number = istream.read_string(header.length);
    asset_tag = istream.read_string(header.length);
    part_number = istream.read_string(header.length);
  }
  if (smbios->version >= smbios_version::SMBIOS_2_5) {
    core_count = istream.sbumpc();
    core_enabled = istream.sbumpc();
    thread_count = istream.sbumpc();
    processor_characteristics = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_6) {
    processor_family_2 = istream.read_uint16();
    core_count_2 = istream.read_uint16();
    core_enabled_2 = istream.read_uint16();
    thread_count_2 = istream.read_uint16();
  }
}

void cache_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_0) {
    socket_designation = istream.read_string(header.length);
    cache_configuration = istream.read_uint16();
    max_cache_size = istream.read_uint16();
    installed_size = istream.read_uint16();
    supported_sram_type = istream.read_uint16();
    current_sram_type = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    cache_speed = istream.sbumpc();
    error_correction_type = istream.sbumpc();
    system_cache_type = istream.sbumpc();
    associativity = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_3_1) {
    max_cache_size2 = istream.read_uint32();
    installed_cache_size2 = istream.read_uint32();
  }
}

void port_connector_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  internal_ref_designator = istream.read_string(header.length);
  internal_connector_type = istream.sbumpc();
  external_ref_designator = istream.read_string(header.length);
  external_connector_type = istream.sbumpc();
  port_type = istream.sbumpc();
}

void oem_strings::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  strings = istream.read_strings(header.length);
}

void sys_configuration_options::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  strings = istream.read_strings(header.length);
}

void physical_memory_arr::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    location = istream.sbumpc();
    use = istream.sbumpc();
    memory_err_correction = istream.sbumpc();
    max_capacity = istream.read_uint32();
    memory_err_info_handle = istream.read_uint16();
    num_of_memory_device = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_7) {
    extended_max_capacity = istream.read_uint64();
  }
}

void memory_err_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    err_type = istream.sbumpc();
    err_granularity = istream.sbumpc();
    err_operation = istream.sbumpc();
    vendor_syndrome = istream.read_uint32();
    memory_arr_err_addr = istream.read_uint32();
    device_err_addr = istream.read_uint32();
    err_resolution = istream.read_uint32();
  }
}

void memory_arr_mapped_addr::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    starting_addr = istream.read_uint32();
    ending_addr = istream.read_uint32();
    memory_arr_handle = istream.read_uint16();
    partition_width = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_7) {
    extended_starting_addr = istream.read_uint64();
    extended_ending_addr = istream.read_uint64();
  }
}

void sys_boot_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  istream.sgetn(reinterpret_cast<char*>(reserved), sizeof(reserved));
  istream.sgetn(reinterpret_cast<char*>(boot_status), sizeof(boot_status));
}

void tpm_device::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  istream.sgetn(reinterpret_cast<char*>(vendor_id), sizeof(vendor_id));
  major_spec_version = istream.sbumpc();
  minor_spec_version = istream.sbumpc();
  firmware_version1 = istream.read_uint32();
  firmware_version2 = istream.read_uint32();
  description = istream.read_strings(header.length);
  characteristics = istream.read_uint64();
  oem_defined = istream.read_uint32();
}

void memory_device::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    physical_memory_arr_handle = istream.read_uint16();
    memory_err_info_handle = istream.read_uint16();
    total_width = istream.read_uint16();
    data_width = istream.read_uint16();
    size = istream.read_uint16();
    form_factor = istream.sbumpc();
    device_set = istream.sbumpc();
    device_locator = istream.read_string(header.length);
    bank_locator = istream.read_string(header.length);
    memory_type = istream.sbumpc();
    type_detail = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_3) {
    speed = istream.read_uint16();
    manufacturer = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
    asset_tag = istream.read_string(header.length);
    part_number = istream.read_string(header.length);
  }
  if (smbios->version >= smbios_version::SMBIOS_2_6) {
    attributes = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_7) {
    extended_size = istream.read_uint32();
    configured_memory_speed = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_8) {
    min_voltage = istream.read_uint16();
    max_voltage = istream.read_uint16();
    configured_voltage = istream.read_uint16();
  }
  if (smbios->version >= smbios_version::SMBIOS_3_2) {
    memory_technology = istream.sbumpc();
    memory_operating_mode_capability = istream.read_uint16();
    firmware_version = istream.read_string(header.length);
    module_manufacturer_id = istream.read_uint16();
    module_product_id = istream.read_uint16();
    memory_subsystem_controller_manufacturer_id = istream.read_uint16();
    memory_subsystem_controller_product_id = istream.read_uint16();
    non_volatile_size = istream.read_uint64();
    volatile_size = istream.read_uint64();
    cache_size = istream.read_uint64();
    logical_size = istream.read_uint64();
  }
  if (smbios->version >= smbios_version::SMBIOS_3_3) {
    extended_speed = istream.read_uint32();
    extended_configured_memory_speed = istream.read_uint32();
  }
  if (smbios->version >= smbios_version::SMBIOS_3_7) {
    pmic0_manufacturer_id = istream.read_uint16();
    pmic0_revision_number = istream.read_uint16();
    rcd_manufacturer_id = istream.read_uint16();
    rcd_revision_number = istream.read_uint16();
  }
}

void processor_additional_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  referenced_handle = istream.read_uint16();
  block_length = istream.sbumpc();
  processor_type = istream.sbumpc();
  processor_specific_data.resize(block_length);
  istream.sgetn(reinterpret_cast<char*>(processor_specific_data.data()), block_length);
}

void memory_device_mapped_address::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    starting_address = istream.read_uint32();
    ending_address = istream.read_uint32();
    memory_device_handle = istream.read_uint16();
    memory_arr_mapped_addr_handle = istream.read_uint16();
    partition_row_position = istream.sbumpc();
    interleave_position = istream.sbumpc();
    interleaved_data_depth = istream.sbumpc();
    extended_starting_address = istream.read_uint64();
    extended_ending_address = istream.read_uint64();
  }
}

void additional_info_entry::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  entry_length = istream.sbumpc();
  referenced_handle = istream.read_uint16();
  referenced_offset = istream.sbumpc();
  string = istream.read_string(header.length);
  value.resize(entry_length - 5);
  istream.sgetn(reinterpret_cast<char*>(value.data()), static_cast<std::streamsize>(value.size()));

  istream.seek_offset();
  istream.sbumpc();

  while (istream.pos() < istream.size()) {
    auto a = istream.sbumpc();
    if (a == 0 && istream.sgetc() == 0) {
      istream.pubseekoff(-1, std::ios_base::cur);
      break;
    } else {
      other.push_back(a);
    }
  }
}

void additional_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  num_of_additional_info_entries = istream.sbumpc();
  for (int i = 0; i < num_of_additional_info_entries; ++i) {
    additional_info_entry entry;
    entry.header.length = header.length;
    entry.read_params(istream, smbios);
    entries.push_back(entry);
  }
}

void peer_device::read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) {
  segment_group_number = istream.read_uint16();
  bus_number = istream.sbumpc();
  device_function_number = istream.sbumpc();
  data_bus_width = istream.sbumpc();
}

void system_slot::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  if (smbios->version >= smbios_version::SMBIOS_2_0) {
    slot_designation = istream.read_string(header.length);
    slot_type = istream.sbumpc();
    slot_data_bus_width = istream.sbumpc();
    current_usage = istream.sbumpc();
    slot_length = istream.sbumpc();
    slot_id = istream.read_uint16();
    slot_characteristics1 = istream.sbumpc();
  }
  if (smbios->version >= smbios_version::SMBIOS_2_1) {
    slot_characteristics2 = istream.sbumpc();
  }
  if (header.length >= 17) {
    segment_group_number = istream.read_uint16();
    bus_number = istream.sbumpc();
    device_function_number = istream.sbumpc();
  }
  if (header.length >= 19) {
    data_bus_width = istream.sbumpc();
    peer_grouping_count = istream.sbumpc();
    if (header.length >= 19 + peer_grouping_count * 5) {
      for (int i = 0; i <= peer_grouping_count; ++i) {
        peer_device device;
        device.read_params(istream, smbios);
        peer_groups.push_back(device);
      }
    }
  }
  if (header.length >= 23 + peer_grouping_count * 5) {
    slot_info = istream.sbumpc();
    slot_physical_width = istream.sbumpc();
    slot_pitch = istream.read_uint16();
  }
  if (header.length >= 24 + peer_grouping_count * 5) {
    slot_height = istream.sbumpc();
  }
}

void onboard_devices_extended_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  reference_designation = istream.read_string(header.length);
  device_type = istream.sbumpc();
  device_type_instance = istream.sbumpc();
  segment_group_number = istream.read_uint16();
  bus_number = istream.sbumpc();
  device_function_number = istream.sbumpc();
}

void firmware_inventory_info::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  firmware_component_name = istream.read_string(header.length);
  firmware_version = istream.read_string(header.length);
  version_format = istream.sbumpc();
  firmware_id = istream.read_string(header.length);
  firmware_id_format = istream.sbumpc();
  release_date = istream.read_string(header.length);
  manufacturer = istream.read_string(header.length);
  lowest_supported_firmware_version = istream.read_string(header.length);
  image_size = istream.read_uint64();
  characteristics = istream.read_uint16();
  state = istream.sbumpc();
  num_of_associated_components = istream.sbumpc();
  for (int i = 0; i < num_of_associated_components; ++i) {
    associated_component_handles.push_back(istream.read_uint16());
  }
}

void unknown_struct::read_params(bios_streambuf& istream, smbios_ctx* smbios) {
  data.resize(header.length - 4);
  istream.sgetn(reinterpret_cast<char*>(data.data()), (std::streamsize) data.size());
  while (istream.pos() < istream.size()) {
    uint8_t a = istream.sbumpc();
    if (a == 0 && istream.sgetc() == 0) {
      istream.pubseekoff(-1, std::ios_base::cur);
      break;
    } else {
      data.push_back(a);
    }
  }
}

std::vector<std::string> unknown_struct::to_strings() {
  std::vector<std::string> strs;
  auto start = data.data() + header.length - 4;
  while (start < data.data() + data.size()) {
    auto count = std::min(data.size() - (start - data.data()), std::strlen((const char*) start) + 1);
    strs.emplace_back((const char*) start, count);
    start += count;
  }
  return strs;
}
