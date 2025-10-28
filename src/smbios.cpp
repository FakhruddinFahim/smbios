//
// Created by Fakhr on 24/05/2024.
//

#include "smbios.hpp"
#include <cstring>
#include <format>
#include <iostream>
#include <iomanip>

#include "smbios_parser.hpp"

#ifdef WIN32

#include <windows.h>

#else
#include <filesystem>
#include <fstream>
#endif

void smbios_header::read(bios_streambuf& istream) {
  type = static_cast<smbios_type>(istream.sbumpc());
  length = istream.sbumpc();
  handle = istream.sbumpc() | istream.sbumpc() << 8;
}

void smbios_entry_point32::read_params(bios_streambuf& istream, smbios_parser* parser) {
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

void smbios_entry_point64::read_params(bios_streambuf& istream, smbios_parser* parser) {
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

bios_characteristics operator|(const bios_characteristics& lhs, const bios_characteristics& rhs) {
  using T = std::underlying_type_t<bios_characteristics>;
  return static_cast<bios_characteristics>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

bios_characteristics operator&(const bios_characteristics& lhs, const bios_characteristics& rhs) {
  using T = std::underlying_type_t<bios_characteristics>;
  return static_cast<bios_characteristics>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

bios_char_ext_byte1 operator|(const bios_char_ext_byte1& lhs, const bios_char_ext_byte1& rhs) {
  using T = std::underlying_type_t<bios_char_ext_byte1>;
  return static_cast<bios_char_ext_byte1>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

bios_char_ext_byte1 operator&(const bios_char_ext_byte1& lhs, const bios_char_ext_byte1& rhs) {
  using T = std::underlying_type_t<bios_char_ext_byte1>;
  return static_cast<bios_char_ext_byte1>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

bios_char_ext_byte2 operator|(const bios_char_ext_byte2& lhs, const bios_char_ext_byte2& rhs) {
  using T = std::underlying_type_t<bios_char_ext_byte2>;
  return static_cast<bios_char_ext_byte2>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

bios_char_ext_byte2 operator&(const bios_char_ext_byte2& lhs, const bios_char_ext_byte2& rhs) {
  using T = std::underlying_type_t<bios_char_ext_byte2>;
  return static_cast<bios_char_ext_byte2>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

extended_bios_rom_size operator|(const extended_bios_rom_size& lhs, const extended_bios_rom_size& rhs) {
  using T = std::underlying_type_t<extended_bios_rom_size>;
  return static_cast<extended_bios_rom_size>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

extended_bios_rom_size operator&(const extended_bios_rom_size& lhs, const extended_bios_rom_size& rhs) {
  using T = std::underlying_type_t<extended_bios_rom_size>;
  return static_cast<extended_bios_rom_size>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void smbios::read(bios_streambuf& istream, smbios_parser* parser) {
  istream.header_start();
  header.read(istream);
  read_params(istream, parser);
  istream.seek_offset();
  char end[2];
  if (istream.sgetn(end, 2) != 2 || end[0] != 0 || end[1] != 0) {
    throw std::runtime_error(std::format("structure terminator not found, type: {}", static_cast<int>(header.type)));
  }
}

void bios_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    vendor = istream.read_string(header.length);
    bios_version = istream.read_string(header.length);
    bios_starting_addr_seg = istream.read_uint16();
    bios_release_date = istream.read_string(header.length);
    bios_rom_size = istream.sbumpc();
    bios_characteristics = static_cast<enum bios_characteristics>(istream.read_uint64());
  }
  if (parser->version == smbios_version::smbios_2_1 || parser->version == smbios_version::smbios_2_2) {
    bios_characteristics_ext_byte1 = static_cast<bios_char_ext_byte1>(istream.sbumpc());
  } else if (parser->version >= smbios_version::smbios_2_3) {
    bios_characteristics_ext_byte1 = static_cast<bios_char_ext_byte1>(istream.sbumpc());
    bios_characteristics_ext_byte2 = static_cast<bios_char_ext_byte2>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_2_4) {
    system_bios_major_release = istream.sbumpc();
    system_bios_minor_release = istream.sbumpc();
    embedded_controller_firmware_major_release = istream.sbumpc();
    embedded_controller_firmware_minor_release = istream.sbumpc();
  }
  if (parser->version >= smbios_version::smbios_3_1) {
    extended_bios_rom_size = static_cast<enum extended_bios_rom_size>(istream.read_uint16());
  }
}

bool bios_info::has_characteristics(const enum bios_characteristics& characteristics) const {
  return (bios_characteristics & characteristics) != static_cast<enum bios_characteristics>(0);
}

bool bios_info::has_characteristics_ext1(const bios_char_ext_byte1& byte1) const {
  return (bios_characteristics_ext_byte1 & byte1) != static_cast<bios_char_ext_byte1>(0);
}

bool bios_info::has_characteristics_ext2(const bios_char_ext_byte2& byte2) const {
  return (bios_characteristics_ext_byte2 & byte2) == byte2;
}

int32_t bios_info::get_extended_bios_rom_size() const {
  return static_cast<int32_t>(extended_bios_rom_size & extended_bios_rom_size::size);
}

wake_up_type operator|(const wake_up_type& lhs, const wake_up_type& rhs) {
  using T = std::underlying_type_t<wake_up_type>;
  return static_cast<wake_up_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

wake_up_type operator&(const wake_up_type& lhs, const wake_up_type& rhs) {
  using T = std::underlying_type_t<wake_up_type>;
  return static_cast<wake_up_type>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void system_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    manufacturer = istream.read_string(header.length);
    product_name = istream.read_string(header.length);
    version = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
  }
  if (parser->version >= smbios_version::smbios_2_1) {
    istream.sgetn(reinterpret_cast<char*>(uuid), sizeof(uuid));
    wake_up_type = static_cast<enum wake_up_type>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_2_4) {
    sku_number = istream.read_string(header.length);
    family = istream.read_string(header.length);
  }
}

feature_flag operator|(const feature_flag& lhs, const feature_flag& rhs) {
  using T = std::underlying_type_t<feature_flag>;
  return static_cast<feature_flag>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

feature_flag operator&(const feature_flag& lhs, const feature_flag& rhs) {
  using T = std::underlying_type_t<feature_flag>;
  return static_cast<feature_flag>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void baseboard_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  manufacturer = istream.read_string(header.length);
  product = istream.read_string(header.length);
  version = istream.read_string(header.length);
  serial_number = istream.read_string(header.length);
  asset_tag = istream.read_string(header.length);
  feature_flags = static_cast<feature_flag>(istream.sbumpc());
  location_in_chassis = istream.read_string(header.length);
  chassis_handle = istream.read_uint16();
  board_type = static_cast<enum board_type>(istream.sbumpc());
  num_of_contained_object_handles = istream.sbumpc();
  for (int i = 0; i < num_of_contained_object_handles; ++i) {
    contained_object_handles.push_back(istream.read_uint16());
  }
}

chassis_type operator|(const chassis_type& lhs, const chassis_type& rhs) {
  using T = std::underlying_type_t<chassis_type>;
  return static_cast<chassis_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

chassis_type operator&(const chassis_type& lhs, const chassis_type& rhs) {
  using T = std::underlying_type_t<chassis_type>;
  return static_cast<chassis_type>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void system_enclosure::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    manufacturer = istream.read_string(header.length);
    type = static_cast<chassis_type>(istream.sbumpc());
    version = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
    asset_tag_number = istream.read_string(header.length);
  }
  if (parser->version >= smbios_version::smbios_2_1) {
    boot_up_state = static_cast<system_enclosure_state>(istream.sbumpc());
    power_supply_state = static_cast<system_enclosure_state>(istream.sbumpc());
    thermal_state = static_cast<system_enclosure_state>(istream.sbumpc());
    security_status = static_cast<system_enclosure_state>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_2_3) {
    oem_defined = istream.read_uint32();
    height = istream.sbumpc();
    num_of_power_cords = istream.sbumpc();
    contained_element_count = istream.sbumpc();
    contained_element_record_length = istream.sbumpc();
    for (int i = 0; i < contained_element_count * contained_element_record_length; ++i) {
      contained_elements.push_back(istream.sbumpc());
    }
  }
  if (parser->version >= smbios_version::smbios_2_7) {
    sku_number = istream.read_string(header.length);
  }
}

processor_characteristics operator|(const processor_characteristics& lhs, const processor_characteristics& rhs) {
  using T = std::underlying_type_t<processor_characteristics>;
  return static_cast<processor_characteristics>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

processor_characteristics operator&(const processor_characteristics& lhs, const processor_characteristics& rhs) {
  using T = std::underlying_type_t<processor_characteristics>;
  return static_cast<processor_characteristics>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

cpu_status& cpu_status::operator=(uint8_t new_value) {
  value = new_value;
  status = static_cast<cpu_status_enum>(value & 0b111);
  socket = static_cast<cpu_socket>(value >> 6 & 0b1);
  return *this;
}

void processor_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    socket_designation = istream.read_string(header.length);
    processor_type = static_cast<enum processor_type>(istream.sbumpc());
    processor_family = static_cast<enum processor_family>(istream.sbumpc());
    processor_manufacturer = istream.read_string(header.length);
    processor_id = istream.read_uint64();
    processor_version = istream.read_string(header.length);
    voltage = istream.sbumpc();
    external_clock = istream.read_uint16();
    max_speed = istream.read_uint16();
    current_speed = istream.read_uint16();
    status = istream.sbumpc();
    processor_upgrade = static_cast<enum processor_upgrade>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_2_1) {
    l1_cache_handle = istream.read_uint16();
    l2_cache_handle = istream.read_uint16();
    l3_cache_handle = istream.read_uint16();
  }
  if (parser->version >= smbios_version::smbios_2_3) {
    serial_number = istream.read_string(header.length);
    asset_tag = istream.read_string(header.length);
    part_number = istream.read_string(header.length);
  }
  if (parser->version >= smbios_version::smbios_2_5) {
    core_count = istream.sbumpc();
    core_enabled = istream.sbumpc();
    thread_count = istream.sbumpc();
    processor_characteristics = static_cast<enum processor_characteristics>(istream.read_uint16());
  }
  if (parser->version >= smbios_version::smbios_2_6) {
    processor_family_2 = static_cast<enum processor_family>(istream.read_uint16());
    core_count_2 = istream.read_uint16();
    core_enabled_2 = istream.read_uint16();
    thread_count_2 = istream.read_uint16();
  }
}

bool processor_info::has_processor_characteristics(const enum processor_characteristics& characteristics) const {
  return (processor_characteristics & characteristics) == characteristics;
}

cache_configuration& cache_configuration::operator=(uint16_t new_value) {
  this->value = new_value;
  level = static_cast<cache_level>(value & 0b111);
  socket = static_cast<cache_socket>(value >> 3 & 0b1);
  location = static_cast<cache_location>(value >> 5 & 0b11);
  state = static_cast<cache_state>(value >> 7 & 0b1);
  operational_mode = static_cast<cache_operational_mode>(value >> 8 & 0b11);
  return *this;
}

cache_size& cache_size::operator=(uint16_t new_value) {
  this->value = new_value;
  size = value & 0x7FFF;
  granularity = static_cast<cache_granularity>(value >> 15 & 0b1);
  return *this;
}

sram_type operator|(const sram_type& lhs, const sram_type& rhs) {
  using T = std::underlying_type_t<sram_type>;
  return static_cast<sram_type>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

sram_type operator&(const sram_type& lhs, const sram_type& rhs) {
  using T = std::underlying_type_t<sram_type>;
  return static_cast<sram_type>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

cache_size2& cache_size2::operator=(uint32_t new_value) {
  this->value = new_value;
  size = value & 0x7FFFFFFF;
  granularity = static_cast<cache_granularity>(value >> 31 & 0b1);
  return *this;
}

void cache_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    socket_designation = istream.read_string(header.length);
    cache_configuration = istream.read_uint16();
    max_cache_size = istream.read_uint16();
    installed_size = istream.read_uint16();
    supported_sram_type = static_cast<sram_type>(istream.read_uint16());
    current_sram_type = static_cast<sram_type>(istream.read_uint16());
  }
  if (parser->version >= smbios_version::smbios_2_1) {
    cache_speed = istream.sbumpc();
    error_correction_type = static_cast<enum error_correction_type>(istream.sbumpc());
    system_cache_type = static_cast<enum system_cache_type>(istream.sbumpc());
    associativity = static_cast<enum associativity>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_3_1) {
    max_cache_size2 = istream.read_uint32();
    installed_cache_size2 = istream.read_uint32();
  }
}

void port_connector_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  internal_ref_designator = istream.read_string(header.length);
  internal_connector_type = static_cast<connector_type>(istream.sbumpc());
  external_ref_designator = istream.read_string(header.length);
  external_connector_type = static_cast<connector_type>(istream.sbumpc());
  port_type = static_cast<enum port_type>(istream.sbumpc());
}

void oem_strings::read_params(bios_streambuf& istream, smbios_parser* parser) {
  strings = istream.read_strings(header.length);
}

void system_configuration_options::read_params(bios_streambuf& istream, smbios_parser* parser) {
  strings = istream.read_strings(header.length);
}

void physical_memory_array::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_1) {
    location = static_cast<memory_array_location>(istream.sbumpc());
    use = static_cast<memory_array_use>(istream.sbumpc());
    memory_error_correction = static_cast<memory_array_error_correction>(istream.sbumpc());
    max_capacity = istream.read_uint32();
    memory_err_info_handle = istream.read_uint16();
    num_of_memory_device = istream.read_uint16();
  }
  if (parser->version >= smbios_version::smbios_2_7) {
    extended_max_capacity = istream.read_uint64();
  }
}

void memory_error_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_1) {
    error_type = static_cast<memory_error_type>(istream.sbumpc());
    error_granularity = static_cast<memory_error_granularity>(istream.sbumpc());
    error_operation = static_cast<memory_error_operation>(istream.sbumpc());
    vendor_syndrome = istream.read_uint32();
    memory_array_error_address = istream.read_uint32();
    device_error_address = istream.read_uint32();
    error_resolution = istream.read_uint32();
  }
}

void memory_array_mapped_address::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_1) {
    starting_address = istream.read_uint32();
    ending_address = istream.read_uint32();
    memory_array_handle = istream.read_uint16();
    partition_width = istream.sbumpc();
  }
  if (parser->version >= smbios_version::smbios_2_7) {
    extended_starting_address = istream.read_uint64();
    extended_ending_address = istream.read_uint64();
  }
}

void system_boot_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  istream.sgetn(reinterpret_cast<char*>(reserved), sizeof(reserved));
  istream.sgetn(reinterpret_cast<char*>(boot_status), sizeof(boot_status));
}

void tpm_device::read_params(bios_streambuf& istream, smbios_parser* parser) {
  istream.sgetn(reinterpret_cast<char*>(vendor_id), sizeof(vendor_id));
  major_spec_version = istream.sbumpc();
  minor_spec_version = istream.sbumpc();
  firmware_version1 = istream.read_uint32();
  firmware_version2 = istream.read_uint32();
  description = istream.read_strings(header.length);
  characteristics = istream.read_uint64();
  oem_defined = istream.read_uint32();
}

memory_device_attributes& memory_device_attributes::operator=(uint8_t new_value) {
  value = new_value;
  rank = value & 0b1111;
  return *this;
}

memory_device_extended_size& memory_device_extended_size::operator=(uint32_t new_value) {
  value = new_value;
  size = new_value & 0x7fffffff;
  return *this;
}

memory_operating_mode_capability operator|(const memory_operating_mode_capability& lhs,
                                           const memory_operating_mode_capability& rhs) {
  using T = std::underlying_type_t<memory_operating_mode_capability>;
  return static_cast<memory_operating_mode_capability>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

memory_operating_mode_capability operator&(const memory_operating_mode_capability& lhs,
                                           const memory_operating_mode_capability& rhs) {
  using T = std::underlying_type_t<memory_operating_mode_capability>;
  return static_cast<memory_operating_mode_capability>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

memory_device_extended_speed& memory_device_extended_speed::operator=(uint32_t new_value) {
  value = new_value;
  speed = new_value & 0x7fffffff;
  return *this;
}

memory_device_type_detail operator|(const memory_device_type_detail& lhs, const memory_device_type_detail& rhs) {
  using T = std::underlying_type_t<memory_device_type_detail>;
  return static_cast<memory_device_type_detail>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

memory_device_type_detail operator&(const memory_device_type_detail& lhs, const memory_device_type_detail& rhs) {
  using T = std::underlying_type_t<memory_device_type_detail>;
  return static_cast<memory_device_type_detail>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void memory_device::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_1) {
    physical_memory_array_handle = istream.read_uint16();
    memory_error_info_handle = istream.read_uint16();
    total_width = istream.read_uint16();
    data_width = istream.read_uint16();
    size = istream.read_uint16();
    form_factor = static_cast<memory_device_form_factor>(istream.sbumpc());
    device_set = istream.sbumpc();
    device_locator = istream.read_string(header.length);
    bank_locator = istream.read_string(header.length);
    memory_type = static_cast<memory_device_type>(istream.sbumpc());
    type_detail = static_cast<memory_device_type_detail>(istream.read_uint16());
  }
  if (parser->version >= smbios_version::smbios_2_3) {
    speed = istream.read_uint16();
    manufacturer = istream.read_string(header.length);
    serial_number = istream.read_string(header.length);
    asset_tag = istream.read_string(header.length);
    part_number = istream.read_string(header.length);
  }
  if (parser->version >= smbios_version::smbios_2_6) {
    attributes = istream.sbumpc();
  }
  if (parser->version >= smbios_version::smbios_2_7) {
    extended_size = istream.read_uint32();
    configured_memory_speed = istream.read_uint16();
  }
  if (parser->version >= smbios_version::smbios_2_8) {
    min_voltage = istream.read_uint16();
    max_voltage = istream.read_uint16();
    configured_voltage = istream.read_uint16();
  }
  if (parser->version >= smbios_version::smbios_3_2) {
    memory_technology = static_cast<enum memory_technology>(istream.sbumpc());
    memory_operating_mode_capability = static_cast<enum memory_operating_mode_capability>(istream.read_uint16());
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
  if (parser->version >= smbios_version::smbios_3_3) {
    extended_speed = istream.read_uint32();
    extended_configured_memory_speed = istream.read_uint32();
  }
  if (parser->version >= smbios_version::smbios_3_7) {
    pmic0_manufacturer_id = istream.read_uint16();
    pmic0_revision_number = istream.read_uint16();
    rcd_manufacturer_id = istream.read_uint16();
    rcd_revision_number = istream.read_uint16();
  }
}

void processor_additional_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  referenced_handle = istream.read_uint16();
  block_length = istream.sbumpc();
  processor_type = istream.sbumpc();
  processor_specific_data.resize(block_length);
  istream.sgetn(reinterpret_cast<char*>(processor_specific_data.data()), block_length);
}

void memory_device_mapped_address::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_1) {
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

void additional_info_entry::read_params(bios_streambuf& istream, smbios_parser* parser) {
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

void additional_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  num_of_additional_info_entries = istream.sbumpc();
  for (int i = 0; i < num_of_additional_info_entries; ++i) {
    additional_info_entry entry;
    entry.header.length = header.length;
    entry.read_params(istream, parser);
    entries.push_back(entry);
  }
}

slot_characteristics1 operator|(const slot_characteristics1& lhs, const slot_characteristics1& rhs) {
  using T = std::underlying_type_t<slot_characteristics1>;
  return static_cast<slot_characteristics1>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

slot_characteristics1 operator&(const slot_characteristics1& lhs, const slot_characteristics1& rhs) {
  using T = std::underlying_type_t<slot_characteristics1>;
  return static_cast<slot_characteristics1>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

slot_characteristics2 operator|(const slot_characteristics2& lhs, const slot_characteristics2& rhs) {
  using T = std::underlying_type_t<slot_characteristics2>;
  return static_cast<slot_characteristics2>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

slot_characteristics2 operator&(const slot_characteristics2& lhs, const slot_characteristics2& rhs) {
  using T = std::underlying_type_t<slot_characteristics2>;
  return static_cast<slot_characteristics2>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void peer_device::read_params(bios_streambuf& istream, smbios_parser* parser) {
  segment_group_number = istream.read_uint16();
  bus_number = istream.sbumpc();
  device_function_number = istream.sbumpc();
  data_bus_width = istream.sbumpc();
}

void system_slots::read_params(bios_streambuf& istream, smbios_parser* parser) {
  if (parser->version >= smbios_version::smbios_2_0) {
    slot_designation = istream.read_string(header.length);
    slot_type = static_cast<system_slot_type>(istream.sbumpc());
    slot_data_bus_width = static_cast<system_slot_width>(istream.sbumpc());
    current_usage = static_cast<system_slot_current_usage>(istream.sbumpc());
    slot_length = static_cast<system_slot_length>(istream.sbumpc());
    slot_id = istream.read_uint16();
    slot_characteristics1 = static_cast<enum slot_characteristics1>(istream.sbumpc());
  }
  if (parser->version >= smbios_version::smbios_2_1) {
    slot_characteristics2 = static_cast<enum slot_characteristics2>(istream.sbumpc());
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
        device.read_params(istream, parser);
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

onboard_device_type& onboard_device_type::operator=(uint8_t new_value) {
  value = new_value;
  type = static_cast<device_type>(value & 0x7F);
  status = static_cast<device_status>(value >> 7);
  return *this;
}

void onboard_devices_extended_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  reference_designation = istream.read_string(header.length);
  device_type = istream.sbumpc();
  device_type_instance = istream.sbumpc();
  segment_group_number = istream.read_uint16();
  bus_number = istream.sbumpc();
  device_function_number = istream.sbumpc();
}

firmware_inventory_characteristics operator|(const firmware_inventory_characteristics& lhs,
                                             const firmware_inventory_characteristics& rhs) {
  using T = std::underlying_type_t<firmware_inventory_characteristics>;
  return static_cast<firmware_inventory_characteristics>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

firmware_inventory_characteristics operator&(const firmware_inventory_characteristics& lhs,
                                             const firmware_inventory_characteristics& rhs) {
  using T = std::underlying_type_t<firmware_inventory_characteristics>;
  return static_cast<firmware_inventory_characteristics>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

void firmware_inventory_info::read_params(bios_streambuf& istream, smbios_parser* parser) {
  firmware_component_name = istream.read_string(header.length);
  firmware_version = istream.read_string(header.length);
  version_format = istream.sbumpc();
  firmware_id = istream.read_string(header.length);
  firmware_id_format = istream.sbumpc();
  release_date = istream.read_string(header.length);
  manufacturer = istream.read_string(header.length);
  lowest_supported_firmware_version = istream.read_string(header.length);
  image_size = istream.read_uint64();
  characteristics = static_cast<firmware_inventory_characteristics>(istream.read_uint16());
  state = istream.sbumpc();
  num_of_associated_components = istream.sbumpc();
  for (int i = 0; i < num_of_associated_components; ++i) {
    associated_component_handles.push_back(istream.read_uint16());
  }
}

void unknown_struct::read_params(bios_streambuf& istream, smbios_parser* parser) {
  data.resize(header.length - 4);
  istream.sgetn(reinterpret_cast<char*>(data.data()), (std::streamsize)data.size());
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

std::vector<std::string> unknown_struct::to_strings() const {
  std::vector<std::string> strs;
  auto start = data.data() + header.length - 4;
  while (start < data.data() + data.size()) {
    auto count = std::min(data.size() - (start - data.data()), std::strlen((const char*)start) + 1);
    strs.emplace_back((const char*)start, count);
    start += count;
  }
  return strs;
}
