//
// Created by Fakhr on 24/05/2024.
//

#ifndef SMBIOS_SMBIOS_HPP
#define SMBIOS_SMBIOS_HPP

#include <string>
#include <cinttypes>
#include <vector>
#include <streambuf>
#include "bios_streambuf.hpp"

//Version
enum class smbios_version : uint32_t {
  smbios_2_0 = 0x0200,
  smbios_2_1 = 0x0201,
  smbios_2_2 = 0x0202,
  smbios_2_3 = 0x0203,
  smbios_2_4 = 0x0204,
  smbios_2_5 = 0x0205,
  smbios_2_6 = 0x0206,
  smbios_2_7 = 0x0207,
  smbios_2_8 = 0x0208,
  smbios_3_0 = 0x0300,
  smbios_3_1 = 0x0301,
  smbios_3_2 = 0x0302,
  smbios_3_3 = 0x0303,
  smbios_3_4 = 0x0304,
  smbios_3_5 = 0x0305,
  smbios_3_7 = 0x0307,
};

inline bool operator==(const smbios_version& a, const smbios_version& b) {
  return static_cast<uint32_t>(a) == static_cast<uint32_t>(b);
}

inline bool operator==(const uint32_t a, const smbios_version& b) {
  return a == static_cast<uint32_t>(b);
}

inline bool operator>=(const uint32_t a, const smbios_version& b) {
  return a >= static_cast<uint32_t>(b);
}

enum class smbios_type : uint8_t {
  bios_information = 0,
  system_information = 1,
  baseboard_information = 2,
  system_enclosure = 3,
  processor_information = 4,
  memory_controller = 5,
  memory_module = 6,
  cache_information = 7,
  port_connector_information = 8,
  system_slots = 9,
  onboard_devices_information = 10,
  oem_strings = 11,
  system_configuration_options = 12,
  bios_language = 13,
  group_associations = 14,
  system_event_log = 15,
  physical_memory_array = 16,
  memory_device = 17,
  memory_error_information_32 = 18,
  memory_array_mapped_address = 19,
  memory_device_mapped_address = 20,
  built_in_pointing_device = 21,
  portable_battery = 22,
  system_reset = 23,
  hardware_security = 24,
  system_power_controls = 25,
  voltage_probe = 26,
  cooling_device = 27,
  temperature_probe = 28,
  electrical_current_probe = 29,
  out_of_band_remote_access = 30,
  boot_integrity_services = 31,
  system_boot_information = 32,
  memory_error_information_64 = 33,
  management_device = 34,
  management_device_component = 35,
  management_device_threshold_data = 36,
  memory_channel = 37,
  ipmi_device_information = 38,
  power_supply = 39,
  additional_information = 40,
  onboard_devices_extended_info = 41,
  management_controller_host_interface = 42,
  tpm_device = 43,
  processor_additional_information = 44,
  firmware_inventory_information = 45,
  string_property = 46,
  inactive = 126,
  end_of_table = 127
};

struct smbios_header {
  smbios_type type{};
  uint8_t length{};
  uint16_t handle{};

  void read(bios_streambuf& istream);
};

struct smbios_parser;

struct smbios {
  smbios_header header;

  void read(bios_streambuf& istream, smbios_parser* parser);

  virtual void read_params(bios_streambuf& istream, smbios_parser* parser) = 0;

  virtual ~smbios() = default;
};

struct smbios_entry_point : smbios {
};

//SMBIOS 2.1 (32-bit) Entry Point structure
struct smbios_entry_point32 : smbios_entry_point {
  uint8_t anchor_string[4]{};
  uint8_t entry_point_structure_checksum{};
  uint8_t entry_point_length{};
  uint8_t smbios_major_version{};
  uint8_t smbios_minor_version{};
  uint16_t max_structure_size{};
  uint8_t entry_point_revision{};
  uint8_t formatted_area[5]{};
  uint8_t intermediate_anchor_string[5]{};
  uint8_t intermediate_checksum{};
  uint16_t structure_table_length{};
  uint32_t structure_table_addr{};
  uint16_t num_of_smbios_structures{};
  uint8_t smbios_bcd_revision{};


  void read_params(bios_streambuf& istream, smbios_parser* parser) override;
};

//SMBIOS 3.0 (64-bit) Entry Point structure
struct smbios_entry_point64 : smbios_entry_point {
  uint8_t anchor_string[5]{};
  uint8_t entry_point_structure_checksum{};
  uint8_t entry_point_length{};
  uint8_t smbios_major_version{};
  uint8_t smbios_minor_version{};
  uint8_t smbios_docrev{};
  uint8_t entry_point_revision{};
  uint8_t reserved{};
  uint32_t structure_table_max_size{};
  uint64_t structure_table_addr{};


  void read_params(bios_streambuf& istream, smbios_parser* parser) override;
};


//7.1 BIOS Information (Type 0)
enum class bios_characteristics : uint64_t {
  reserved_0 = 1ull << 0,
  reserved_1 = 1ull << 1,
  unknown = 1ull << 2,
  not_supported = 1ull << 3,
  isa_supported = 1ull << 4,
  mca_supported = 1ull << 5,
  eisa_supported = 1ull << 6,
  pci_supported = 1ull << 7,
  pc_card_supported = 1ull << 8,
  plug_and_play_supported = 1ull << 9,
  apm_supported = 1ull << 10,
  bios_upgradeable = 1ull << 11,
  bios_shadowing_allowed = 1ull << 12,
  vl_vesa_supported = 1ull << 13,
  escd_supported = 1ull << 14,
  boot_from_cd_supported = 1ull << 15,
  selectable_boot_supported = 1ull << 16,
  bios_rom_socketed = 1ull << 17,
  boot_from_pc_card_supported = 1ull << 18,
  edd_specification_supported = 1ull << 19,
  japanese_floppy_nec_9800_supported = 1ull << 20,
  japanese_floppy_toshiba_supported = 1ull << 21,
  floppy_5_25_360kb_supported = 1ull << 22,
  floppy_5_25_1_2mb_supported = 1ull << 23,
  floppy_3_5_720kb_supported = 1ull << 24,
  floppy_3_5_2_88mb_supported = 1ull << 25,
  print_screen_service_supported = 1ull << 26,
  keyboard_service_supported = 1ull << 27,
  serial_service_supported = 1ull << 28,
  printer_service_supported = 1ull << 29,
  video_service_supported = 1ull << 30,
  nec_pc_98 = 1ull << 31,
  reserved_for_bios_vendor = 0xffffull << 32,
  reserved_for_system_vendor = 0xffffull << 48
};

bios_characteristics operator|(const bios_characteristics& lhs, const bios_characteristics& rhs);

bios_characteristics operator&(const bios_characteristics& lhs, const bios_characteristics& rhs);

enum class bios_char_ext_byte1 : uint8_t {
  acpi_supported = 1 << 0,
  usb_legacy_supported = 1 << 1,
  agp_supported = 1 << 2,
  i2o_boot_supported = 1 << 3,
  ls_120_boot_supported = 1 << 4,
  atapi_zip_boot_supported = 1 << 5,
  ieee_1394_boot_supported = 1 << 6,
  smart_battery_supported = 1 << 7
};

bios_char_ext_byte1 operator|(const bios_char_ext_byte1& lhs, const bios_char_ext_byte1& rhs);

bios_char_ext_byte1 operator&(const bios_char_ext_byte1& lhs, const bios_char_ext_byte1& rhs);

enum class bios_char_ext_byte2 : uint8_t {
  bios_boot_specification_supported = 1 << 0,
  function_key_network_service_boot_supported = 1 << 1,
  targeted_content_distribution_enabled = 1 << 2,
  uefi_specification_supported = 1 << 3,
  smbios_describes_virtual_machine = 1 << 4,
  reserved = 0xE0
};

bios_char_ext_byte2 operator|(const bios_char_ext_byte2& lhs, const bios_char_ext_byte2& rhs);

bios_char_ext_byte2 operator&(const bios_char_ext_byte2& lhs, const bios_char_ext_byte2& rhs);

enum class extended_bios_rom_size : uint16_t {
  size = 0x3FFF,
  megabytes = 0 << 14,
  gigabytes = 0b01 << 14,
  reserved = 0b10 << 14,
  reserved2 = 0b11 << 14,
};

extended_bios_rom_size operator|(const extended_bios_rom_size& lhs, const extended_bios_rom_size& rhs);

extended_bios_rom_size operator&(const extended_bios_rom_size& lhs, const extended_bios_rom_size& rhs);


struct bios_info : smbios {
  static constexpr auto TYPE = smbios_type::bios_information;
  //version 2.0+
  std::string vendor{};
  std::string bios_version{};
  uint16_t bios_starting_addr_seg{};
  std::string bios_release_date{};
  uint8_t bios_rom_size{}; //kb = (bios_rom_size + 1) * 64k
  bios_characteristics bios_characteristics{};
  //version 2.4+
  bios_char_ext_byte1 bios_characteristics_ext_byte1;
  bios_char_ext_byte2 bios_characteristics_ext_byte2;
  uint8_t system_bios_major_release{};
  uint8_t system_bios_minor_release{};
  uint8_t embedded_controller_firmware_major_release{};
  uint8_t embedded_controller_firmware_minor_release{};
  //version 3.1+
  extended_bios_rom_size extended_bios_rom_size{};

  void read_params(bios_streambuf& istream, smbios_parser* parser) override;

  [[nodiscard]] bool has_characteristics(const enum bios_characteristics& characteristics) const;

  [[nodiscard]] bool has_characteristics_ext1(const bios_char_ext_byte1& byte1) const;

  [[nodiscard]] bool has_characteristics_ext2(const bios_char_ext_byte2& byte2) const;

  [[nodiscard]] int32_t get_extended_bios_rom_size() const;
};


//System Information (Type 1) structure
enum class wake_up_type : uint8_t {
  reserved = 0x00,
  other = 0x01,
  unknown = 0x02,
  apm_timer = 0x03,
  modem_ring = 0x04,
  lan_remote = 0x05,
  power_switch = 0x06,
  pci_pme = 0x07,
  ac_power_restored = 0x08
};

wake_up_type operator|(const wake_up_type& lhs, const wake_up_type& rhs);

wake_up_type operator&(const wake_up_type& lhs, const wake_up_type& rhs);

struct system_info : smbios {
  static constexpr auto TYPE = smbios_type::system_information;
  //version 2.0+
  std::string manufacturer;
  std::string product_name;
  std::string version;
  std::string serial_number;
  //version 2.1+
  uint8_t uuid[16]{};
  wake_up_type wake_up_type{};
  //version 2.4+
  std::string sku_number;
  std::string family;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class feature_flag : uint8_t {
  hosting_board = 1 << 0,
  requires_auxiliary_cards = 1 << 1,

  removable_board = 1 << 2,
  replaceable_board = 1 << 3,
  hot_swappable_board = 1 << 4,
  reserved = 0xE0
};

feature_flag operator|(const feature_flag& lhs, const feature_flag& rhs);

feature_flag operator&(const feature_flag& lhs, const feature_flag& rhs);

enum class board_type : uint8_t {
  unknown = 0x01,
  other = 0x02,
  server_blade = 0x03,
  connectivity_switch = 0x04,
  system_management_module = 0x05,
  processor_module = 0x06,
  io_module = 0x07,
  memory_module = 0x08,
  daughter_board = 0x09,
  motherboard = 0x0A,
  processor_memory_module = 0x0B,
  processor_io_module = 0x0C,
  interconnect_board = 0x0D
};

//Baseboard (or Module) Information (Type 2)
struct baseboard_info : smbios {
  static constexpr auto TYPE = smbios_type::baseboard_information;
  std::string manufacturer;
  std::string product;
  std::string version;
  std::string serial_number;
  std::string asset_tag;
  feature_flag feature_flags{};
  std::string location_in_chassis;
  uint16_t chassis_handle{};
  board_type board_type{};
  uint8_t num_of_contained_object_handles{};
  std::vector<uint16_t> contained_object_handles;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class chassis_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  desktop = 0x03,
  low_profile_desktop = 0x04,
  pizza_box = 0x05,
  mini_tower = 0x06,
  tower = 0x07,
  portable = 0x08,
  laptop = 0x09,
  notebook = 0x0A,
  hand_held = 0x0B,
  docking_station = 0x0C,
  all_in_one = 0x0D,
  sub_notebook = 0x0E,
  space_saving = 0x0F,
  lunch_box = 0x10,
  main_server_chassis = 0x11,
  expansion_chassis = 0x12,
  sub_chassis = 0x13,
  bus_expansion_chassis = 0x14,
  peripheral_chassis = 0x15,
  raid_chassis = 0x16,
  rack_mount_chassis = 0x17,
  sealed_case_pc = 0x18,
  multi_system_chassis = 0x19,
  compact_pci = 0x1A,
  advanced_tca = 0x1B,
  blade = 0x1C,
  blade_enclosure = 0x1D,
  tablet = 0x1E,
  convertible = 0x1F,
  detachable = 0x20,
  iot_gateway = 0x21,
  embedded_pc = 0x22,
  mini_pc = 0x23,
  stick_pc = 0x24,
  chassis_lock_present = 1 << 7,
};

chassis_type operator|(const chassis_type& lhs, const chassis_type& rhs);

chassis_type operator&(const chassis_type& lhs, const chassis_type& rhs);

enum class system_enclosure_state : uint8_t {
  other = 0x01,
  unknown = 0x02,
  safe = 0x03,
  warning = 0x04,
  critical = 0x05,
  non_recoverable = 0x06
};

enum class security_status : uint8_t {
  other = 0x01,
  unknown = 0x02,
  none = 0x03,
  external_interface_locked_out = 0x04,
  external_interface_enabled = 0x05
};

//System Enclosure or Chassis (Type 3) structure
struct system_enclosure : smbios {
  static constexpr auto TYPE = smbios_type::system_enclosure;
  //2.0+
  std::string manufacturer;
  chassis_type type{};
  std::string version;
  std::string serial_number;
  std::string asset_tag_number;
  //2.1+
  system_enclosure_state boot_up_state{};
  system_enclosure_state power_supply_state{};
  system_enclosure_state thermal_state{};
  system_enclosure_state security_status{};
  //2.3+
  uint32_t oem_defined{};
  uint8_t height{};
  uint8_t num_of_power_cords{};
  uint8_t contained_element_count{};
  uint8_t contained_element_record_length{};
  std::vector<uint8_t> contained_elements;
  //2.7+
  std::string sku_number;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class processor_upgrade : uint8_t {
  other = 0x01,
  unknown = 0x02,
  daughter_board = 0x03,
  zif_socket = 0x04,
  replaceable_piggy_back = 0x05,
  none = 0x06,
  lif_socket = 0x07,
  slot_1 = 0x08,
  slot_2 = 0x09,
  socket_370_pin = 0x0A,
  slot_a = 0x0B,
  slot_m = 0x0C,
  socket_423 = 0x0D,
  socket_a_462 = 0x0E,
  socket_478 = 0x0F,
  socket_754 = 0x10,
  socket_940 = 0x11,
  socket_939 = 0x12,
  socket_mpga604 = 0x13,
  socket_lga771 = 0x14,
  socket_lga775 = 0x15,
  socket_s1 = 0x16,
  socket_am2 = 0x17,
  socket_f_1207 = 0x18,
  socket_lga1366 = 0x19,
  socket_g34 = 0x1A,
  socket_am3 = 0x1B,
  socket_c32 = 0x1C,
  socket_lga1156 = 0x1D,
  socket_lga1567 = 0x1E,
  socket_pga988a = 0x1F,
  socket_bga1288 = 0x20,
  socket_rpga988b = 0x21,
  socket_bga1023 = 0x22,
  socket_bga1224 = 0x23,
  socket_lga1155 = 0x24,
  socket_lga1356 = 0x25,
  socket_lga2011 = 0x26,
  socket_fs1 = 0x27,
  socket_fs2 = 0x28,
  socket_fm1 = 0x29,
  socket_fm2 = 0x2A,
  socket_lga2011_3 = 0x2B,
  socket_lga1356_3 = 0x2C,
  socket_lga1150 = 0x2D,
  socket_bga1168 = 0x2E,
  socket_bga1234 = 0x2F,
  socket_bga1364 = 0x30,
  socket_am4 = 0x31,
  socket_lga1151 = 0x32,
  socket_bga1356 = 0x33,
  socket_bga1440 = 0x34,
  socket_bga1515 = 0x35,
  socket_lga3647_1 = 0x36,
  socket_sp3 = 0x37,
  socket_sp3r2 = 0x38,
  socket_lga2066 = 0x39,
  socket_bga1392 = 0x3A,
  socket_bga1510 = 0x3B,
  socket_bga1528 = 0x3C,
  socket_lga4189 = 0x3D,
  socket_lga1200 = 0x3E
};

enum class processor_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  central_processor = 0x03,
  math_processor = 0x04,
  dsp_processor = 0x05,
  video_processor = 0x06
};

enum class processor_family : uint16_t {
  other = 0x01,
  unknown = 0x02,
  intel_8086 = 0x03,
  intel_80286 = 0x04,
  intel_386 = 0x05,
  intel_486 = 0x06,
  intel_8087 = 0x07,
  intel_80287 = 0x08,
  intel_80387 = 0x09,
  intel_80487 = 0x0A,
  intel_pentium = 0x0B,
  intel_pentium_pro = 0x0C,
  intel_pentium_ii = 0x0D,
  intel_pentium_mmx = 0x0E,
  intel_celeron = 0x0F,
  intel_pentium_ii_xeon = 0x10,
  intel_pentium_iii = 0x11,
  m1_family = 0x12,
  m2_family = 0x13,
  intel_celeron_m = 0x14,
  intel_pentium_4_ht = 0x15,
  amd_duron = 0x18,
  amd_k5 = 0x19,
  amd_k6 = 0x1A,
  amd_k6_2 = 0x1B,
  amd_k6_3 = 0x1C,
  amd_athlon = 0x1D,
  amd_29000 = 0x1E,
  amd_k6_2_plus = 0x1F,
  powerpc_family = 0x20,
  powerpc_601 = 0x21,
  powerpc_603 = 0x22,
  powerpc_603_plus = 0x23,
  powerpc_604 = 0x24,
  powerpc_620 = 0x25,
  powerpc_x704 = 0x26,
  powerpc_750 = 0x27,
  intel_core_duo = 0x28,
  intel_core_duo_mobile = 0x29,
  intel_core_solo_mobile = 0x2A,
  intel_atom = 0x2B,
  intel_core_m = 0x2C,
  intel_core_m3 = 0x2D,
  intel_core_m5 = 0x2E,
  intel_core_m7 = 0x2F,
  alpha_family = 0x30,
  alpha_21064 = 0x31,
  alpha_21066 = 0x32,
  alpha_21164 = 0x33,
  alpha_21164pc = 0x34,
  alpha_21164a = 0x35,
  alpha_21264 = 0x36,
  alpha_21364 = 0x37,
  amd_turion_ii_ultra_dual_core_mobile_m = 0x38,
  amd_turion_ii_dual_core_mobile_m = 0x39,
  amd_athlon_ii_dual_core_mobile_m = 0x3A,
  amd_opteron_6100 = 0x3B,
  amd_opteron_4100 = 0x3C,
  amd_opteron_6200 = 0x3D,
  amd_opteron_4200 = 0x3E,
  amd_fx_series = 0x3F,
  mips_family = 0x40,
  mips_r4000 = 0x41,
  mips_r4200 = 0x42,
  mips_r4400 = 0x43,
  mips_r4600 = 0x44,
  mips_r10000 = 0x45,
  amd_c_series = 0x46,
  amd_e_series = 0x47,
  amd_a_series = 0x48,
  amd_g_series = 0x49,
  amd_z_series = 0x4A,
  amd_r_series = 0x4B,
  amd_opteron_4300 = 0x4C,
  amd_opteron_6300 = 0x4D,
  amd_opteron_3300 = 0x4E,
  amd_firepro = 0x4F,
  sparc_family = 0x50,
  supersparc = 0x51,
  microsparc_ii = 0x52,
  microsparc_iiep = 0x53,
  ultrasparc = 0x54,
  ultrasparc_ii = 0x55,
  ultrasparc_iii = 0x56,
  ultrasparc_iii_plus = 0x57,
  ultrasparc_iii_i = 0x58,
  family_68040 = 0x60,
  family_68xxx = 0x61,
  family_68000 = 0x62,
  family_68010 = 0x63,
  family_68020 = 0x64,
  family_68030 = 0x65,
  amd_athlon_x4_quad_core = 0x66,
  amd_opteron_x1000 = 0x67,
  amd_opteron_x2000_apu = 0x68,
  amd_opteron_a_series = 0x69,
  amd_opteron_x3000_apu = 0x6A,
  amd_zen = 0x6B,
  hobbit_family = 0x70,
  crusoe_tm5000 = 0x78,
  crusoe_tm3000 = 0x79,
  efficeon_tm8000 = 0x7A,
  weitek = 0x80,
  itanium = 0x82,
  amd_athlon_64 = 0x83,
  amd_opteron = 0x84,
  amd_sempron = 0x85,
  amd_turion_64_mobile = 0x86,
  amd_opteron_dual_core = 0x87,
  amd_athlon_64_x2_dual_core = 0x88,
  amd_turion_64_x2_mobile = 0x89,
  amd_opteron_quad_core = 0x8A,
  amd_opteron_third_generation = 0x8B,
  amd_phenom_fx_quad_core = 0x8C,
  amd_phenom_x4_quad_core = 0x8D,
  amd_phenom_x2_dual_core = 0x8E,
  amd_athlon_x2_dual_core = 0x8F,
  pa_risc_family = 0x90,
  pa_risc_8500 = 0x91,
  pa_risc_8000 = 0x92,
  pa_risc_7300lc = 0x93,
  pa_risc_7200 = 0x94,
  pa_risc_7100lc = 0x95,
  pa_risc_7100 = 0x96,
  v30_family = 0xA0,
};

enum class core_count : uint16_t {
  unknown = 0x0000,
  count32 = 0x0020,
  count255 = 0x00FF,
  count256 = 0x0100,
  count512 = 0x0200,
  reserved = 0xFFFF
};

enum class processor_characteristics : uint16_t {
  reserved = 0,
  unknown = 1 << 1,
  bit64_capable = 1 << 2,
  multicore = 1 << 3,
  hardware_thread = 1 << 4,
  execute_protection = 1 << 5,
  enhanced_virtualization = 1 << 6,
  power_performance_control = 1 << 7,
  bit32_capable = 1 << 8,
  arm64_soc_id = 1 << 9,
  reserved2 = 0xFC00
};

processor_characteristics operator|(const processor_characteristics& lhs, const processor_characteristics& rhs);

processor_characteristics operator&(const processor_characteristics& lhs, const processor_characteristics& rhs);

enum class cpu_status_enum : uint8_t {
  unknown = 0,
  cpu_enabled = 0x1,
  cpu_disabled_by_user = 0x2,
  cpu_disabled_by_bios = 0x3,
  idle = 0x4,
  reserved = 0x5,
  reserved2 = 0x6,
  other = 0x7,
};

enum class cpu_socket {
  socket_unpopulated = 0,
  socket_populated = 1,
};

struct cpu_status {
  uint8_t value{};
  cpu_status_enum status{};
  cpu_socket socket{};

  cpu_status& operator=(uint8_t new_value);
};

//Processor Information (Type 4)
struct processor_info : smbios {
  static constexpr auto TYPE = smbios_type::processor_information;
  //2.0+
  std::string socket_designation;
  processor_type processor_type{};
  processor_family processor_family{};
  std::string processor_manufacturer;
  uint64_t processor_id{};
  std::string processor_version;
  uint8_t voltage{};
  uint16_t external_clock{};
  uint16_t max_speed{};
  uint16_t current_speed{};
  cpu_status status{};
  processor_upgrade processor_upgrade{};
  //2.1+
  uint16_t l1_cache_handle{};
  uint16_t l2_cache_handle{};
  uint16_t l3_cache_handle{};
  //2.3+
  std::string serial_number;
  std::string asset_tag;
  std::string part_number;
  //2.5+
  uint8_t core_count{};
  uint8_t core_enabled{};
  uint8_t thread_count{};
  processor_characteristics processor_characteristics{};
  //2.6+
  enum processor_family processor_family_2{};
  //3.0+
  uint16_t core_count_2{};
  uint16_t core_enabled_2{};
  uint16_t thread_count_2{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;

  [[nodiscard]] bool has_processor_characteristics(const enum processor_characteristics& characteristics) const;
};

enum class cache_level : uint8_t {
  l1 = 0b000,
  l2 = 0b001,
  l3 = 0b010,
  l4 = 0b011,
  l5 = 0b100,
  l6 = 0b101,
  l7 = 0b110,
  l8 = 0b111
};

enum class cache_socket : uint8_t {
  not_socketed = 0,
  socketed = 1
};

enum class cache_location : uint8_t {
  internal = 0b00,
  external = 0b01,
  reserved = 0b10,
  unknown = 0b11
};


enum class cache_state : uint8_t {
  disabled = 0,
  enabled = 1
};

enum class cache_operational_mode : uint8_t {
  write_through = 0b00,
  write_back = 0b01,
  varies_with_address = 0b10,
  unknown = 0b11
};

struct cache_configuration {
  uint16_t value{};
  cache_level level{};
  cache_socket socket{};
  cache_location location{};
  cache_state state{};
  cache_operational_mode operational_mode{};

  cache_configuration& operator=(uint16_t new_value);
};

enum class cache_granularity {
  granularity_1k = 0,
  granularity_64k = 1
};

struct cache_size {
  uint16_t value{};
  uint16_t size{};
  cache_granularity granularity{};

  cache_size& operator=(uint16_t new_value);
};

enum class sram_type : uint16_t {
  other = 1 << 0,
  unknown = 1 << 1,
  non_burst = 1 << 2,
  burst = 1 << 3,
  pipeline_burst = 1 << 4,
  synchronous = 1 << 5,
  asynchronous = 1 << 6,
  reserved = 0xFF80
};

sram_type operator|(const sram_type& lhs, const sram_type& rhs);

sram_type operator&(const sram_type& lhs, const sram_type& rhs);

enum class error_correction_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  none = 0x03,
  parity = 0x04,
  single_bit_ecc = 0x05,
  multi_bit_ecc = 0x06
};

enum class system_cache_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  instruction = 0x03,
  data = 0x04,
  unified = 0x05
};

enum class associativity : uint8_t {
  other = 0x01,
  unknown = 0x02,
  direct_mapped = 0x03,
  set_associative_2_way = 0x04,
  set_associative_4_way = 0x05,
  fully_associative = 0x06,
  set_associative_8_way = 0x07,
  set_associative_16_way = 0x08,
  set_associative_12_way = 0x09,
  set_associative_24_way = 0x0A,
  set_associative_32_way = 0x0B,
  set_associative_48_way = 0x0C,
  set_associative_64_way = 0x0D,
  set_associative_20_way = 0x0E
};

struct cache_size2 {
  uint32_t value{};
  uint32_t size{};
  cache_granularity granularity{};

  cache_size2& operator=(uint32_t new_value);
};

//7.8 Cache Information (Type 7)
struct cache_info : smbios {
  static constexpr auto TYPE = smbios_type::cache_information;
  //2.0+
  std::string socket_designation;
  cache_configuration cache_configuration{};
  cache_size max_cache_size{};
  cache_size installed_size{};
  sram_type supported_sram_type{};
  sram_type current_sram_type{};
  //2.1+
  uint8_t cache_speed{};
  error_correction_type error_correction_type{};
  system_cache_type system_cache_type{};
  associativity associativity{};
  //3.1+
  cache_size2 max_cache_size2{};
  cache_size2 installed_cache_size2{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};


// SMBIOS Table 42 – Port Connector Types
enum class connector_type : uint8_t {
  none = 0x00,
  centronics = 0x01,
  mini_centronics = 0x02,
  proprietary = 0x03,
  db25_pin_male = 0x04,
  db25_pin_female = 0x05,
  db15_pin_male = 0x06,
  db15_pin_female = 0x07,
  db9_pin_male = 0x08,
  db9_pin_female = 0x09,
  rj11 = 0x0A,
  rj45 = 0x0B,
  pin50_mini_scsi = 0x0C,
  mini_din = 0x0D,
  micro_din = 0x0E,
  ps2 = 0x0F,
  infrared = 0x10,
  hp_hil = 0x11,
  access_bus_usb = 0x12,
  ssa_scsi = 0x13,
  circular_din_8_male = 0x14,
  circular_din_8_female = 0x15,
  onboard_ide = 0x16,
  onboard_floppy = 0x17,
  pin9_dual_inline_pin10_cut = 0x18,
  pin25_dual_inline_pin26_cut = 0x19,
  pin50_dual_inline = 0x1A,
  pin68_dual_inline = 0x1B,
  onboard_sound_input_from_cdrom = 0x1C,
  mini_centronics_type14 = 0x1D,
  mini_centronics_type26 = 0x1E,
  mini_jack_headphones = 0x1F,
  bnc = 0x20,
  ieee1394 = 0x21,
  sas_sata_plug_receptacle = 0x22,
  usb_type_c_receptacle = 0x23,
  pc98 = 0xA0,
  pc98_hireso = 0xA1,
  pc_h98 = 0xA2,
  pc98_note = 0xA3,
  pc98_full = 0xA4,
  other = 0xFF
};

// SMBIOS Table 43 – Port Types field
enum class port_type : uint8_t {
  none = 0x00,
  parallel_port_xt_at_compatible = 0x01,
  parallel_port_ps2 = 0x02,
  parallel_port_ecp = 0x03,
  parallel_port_epp = 0x04,
  parallel_port_ecp_epp = 0x05,
  serial_port_xt_at_compatible = 0x06,
  serial_port_16450_compatible = 0x07,
  serial_port_16550_compatible = 0x08,
  serial_port_16550a_compatible = 0x09,
  scsi_port = 0x0A,
  midi_port = 0x0B,
  joy_stick_port = 0x0C,
  keyboard_port = 0x0D,
  mouse_port = 0x0E,
  ssa_scsi = 0x0F,
  usb = 0x10,
  firewire_ieee_1394 = 0x11,
  pcmcia_type_i2 = 0x12,
  pcmcia_type_ii = 0x13,
  pcmcia_type_iii = 0x14,
  cardbus = 0x15,
  access_bus_port = 0x16,
  scsi_ii = 0x17,
  scsi_wide = 0x18,
  pc_98 = 0x19,
  pc_98_hireso = 0x1A,
  pc_h98 = 0x1B,
  video_port = 0x1C,
  audio_port = 0x1D,
  modem_port = 0x1E,
  network_port = 0x1F,
  sata = 0x20,
  sas = 0x21,
  mfdp_multi_function_display_port = 0x22,
  thunderbolt = 0x23,
  compatible_8251 = 0xA0,
  compatible_8251_fifo = 0xA1,
  other = 0xFF
};

//7.9 Port Connector Information (Type 8)
struct port_connector_info : smbios {
  static constexpr auto TYPE = smbios_type::port_connector_information;
  std::string internal_ref_designator;
  connector_type internal_connector_type{};
  std::string external_ref_designator;
  connector_type external_connector_type{};
  port_type port_type{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//7.12 OEM Strings (Type 11)
struct oem_strings : smbios {
  static constexpr auto TYPE = smbios_type::oem_strings;
  std::vector<std::string> strings;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};


struct system_configuration_options : smbios {
  static constexpr auto TYPE = smbios_type::system_configuration_options;
  std::vector<std::string> strings;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class memory_array_location : uint8_t {
  other = 0x01,
  unknown = 0x02,
  system_board_or_motherboard = 0x03,
  isa_addon_card = 0x04,
  eisa_addon_card = 0x05,
  pci_addon_card = 0x06,
  mca_addon_card = 0x07,
  pcmcia_addon_card = 0x08,
  proprietary_addon_card = 0x09,
  nubus_addon_card = 0x0A,
  pc98_c20_addon_card = 0xA0,
  pc98_c24_addon_card = 0xA1,
  pc98_e_addon_card = 0xA2,
  pc98_local_addon_card = 0xA3,
  cxl_addon_card = 0xA4
};

// SMBIOS Table 73 – Memory Array: Use field
enum class memory_array_use : uint8_t {
  other = 0x01,
  unknown = 0x02,
  system_memory = 0x03,
  video_memory = 0x04,
  flash_memory = 0x05,
  non_volatile_ram = 0x06,
  cache_memory = 0x07
};

// SMBIOS Table 74 – Memory Array: Error Correction Types field
enum class memory_array_error_correction : uint8_t {
  other = 0x01,
  unknown = 0x02,
  none = 0x03,
  parity = 0x04,
  single_bit_ecc = 0x05,
  multi_bit_ecc = 0x06,
  crc = 0x07
};

//7.17 Physical Memory Array (Type 16)
struct physical_memory_array : smbios {
  static constexpr auto TYPE = smbios_type::physical_memory_array;
  //2.1+
  memory_array_location location{};
  memory_array_use use{};
  memory_array_error_correction memory_error_correction{};
  uint32_t max_capacity{};
  uint16_t memory_err_info_handle{};
  uint16_t num_of_memory_device{};
  //2.7+
  uint64_t extended_max_capacity{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class memory_error_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  ok = 0x03,
  bad_read = 0x04,
  parity_err = 0x05,
  single_bit_err = 0x06,
  double_bit_err = 0x07,
  multi_bit_err = 0x08,
  nibble_err = 0x09,
  checksum_err = 0x0A,
  crc_err = 0x0B,
  corrected_single_bit_err = 0x0C,
  corrected_err = 0x0D,
  uncorrectable_err = 0x0E,
};

enum class memory_error_granularity : uint8_t {
  other = 0x01,
  unknown = 0x02,
  device_level = 0x03,
  memory_partition_level = 0x04,
};

enum class memory_error_operation : uint8_t {
  other = 0x01,
  unknown = 0x02,
  read = 0x03,
  write = 0x04,
  partial_write = 0x05,
};

//7.19 32-Bit Memory Error Information (Type 18)
struct memory_error_info : smbios {
  static constexpr auto TYPE = smbios_type::memory_error_information_32;
  //2.1+
  memory_error_type error_type{};
  memory_error_granularity error_granularity{};
  memory_error_operation error_operation{};
  uint32_t vendor_syndrome{};
  uint32_t memory_array_error_address{};
  uint32_t device_error_address{};
  uint32_t error_resolution{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//7.20 Memory Array Mapped Address (Type 19)
struct memory_array_mapped_address : smbios {
  static constexpr auto TYPE = smbios_type::memory_array_mapped_address;
  //2.1+
  uint32_t starting_address{};
  uint32_t ending_address{};
  uint16_t memory_array_handle{};
  uint8_t partition_width{};
  //2.7+
  uint64_t extended_starting_address{};
  uint64_t extended_ending_address{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//7.33 System Boot Information (Type 32)
struct system_boot_info : smbios {
  static constexpr auto TYPE = smbios_type::system_boot_information;
  uint8_t reserved[6]{};
  uint8_t boot_status[10]{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};


//7.44 TPM Device (Type 43)
struct tpm_device : smbios {
  static constexpr auto TYPE = smbios_type::tpm_device;
  uint8_t vendor_id[4]{};
  uint8_t major_spec_version{};
  uint8_t minor_spec_version{};
  uint32_t firmware_version1{};
  uint32_t firmware_version2{};
  std::vector<std::string> description;
  uint64_t characteristics{};
  uint32_t oem_defined{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class memory_device_form_factor : uint8_t {
  other = 0x01,
  unknown = 0x02,
  simm = 0x03,
  sip = 0x04,
  chip = 0x05,
  dip = 0x06,
  zip = 0x07,
  proprietary_card = 0x08,
  dimm = 0x09,
  tsop = 0x0A,
  row_of_chips = 0x0B,
  rimm = 0x0C,
  sodimm = 0x0D,
  srimm = 0x0E,
  fb_dimm = 0x0F,
  die = 0x10,
};

// SMBIOS Table 77 – Memory Device: Type field
enum class memory_device_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  dram = 0x03,
  edram = 0x04,
  vram = 0x05,
  sram = 0x06,
  ram = 0x07,
  rom = 0x08,
  flash = 0x09,
  eeprom = 0x0A,
  feprom = 0x0B,
  eprom = 0x0C,
  cdram = 0x0D,
  dram3d = 0x0E,
  sdram = 0x0F,
  sgram = 0x10,
  rdram = 0x11,
  ddr = 0x12,
  ddr2 = 0x13,
  ddr2_fb_dimm = 0x14,
  reserved = 0x15,
  ddr3 = 0x18,
  fbd2 = 0x19,
  ddr4 = 0x1A,
  lpddr = 0x1B,
  lpddr2 = 0x1C,
  lpddr3 = 0x1D,
  lpddr4 = 0x1E,
  logical = 0x1F,
  hbm = 0x20,
  hbm2 = 0x21,
  ddr5 = 0x22,
  lpddr5 = 0x23,
};

enum class memory_device_type_detail : uint16_t {
  reserved = 0x0000,
  other = 1 << 1,
  unknown = 1 << 2,
  fast_paged = 1 << 3,
  static_column = 1 << 4,
  pseudo_static = 1 << 5,
  rambus = 1 << 6,
  synchronous = 1 << 7,
  cmos = 1 << 8,
  edo = 1 << 9,
  window_dram = 1 << 10,
  cache_dram = 1 << 11,
  non_volatile = 1 << 12,
  registered = 1 << 13,
  unbuffered = 1 << 14,
  lrdimm = 1 << 15
};

memory_device_type_detail operator|(const memory_device_type_detail& lhs, memory_device_type_detail& rhs);

memory_device_type_detail operator&(const memory_device_type_detail& lhs, memory_device_type_detail& rhs);

struct memory_device_attributes {
  uint8_t value{};
  uint8_t rank{};

  memory_device_attributes& operator=(uint8_t new_value);
};

struct memory_device_extended_size {
  uint32_t value{};
  // Size in megabytes
  uint32_t size{};

  memory_device_extended_size& operator=(uint32_t new_value);
};

enum class memory_technology : uint8_t {
  other = 0x01,
  unknown = 0x02,
  dram = 0x03,
  nvdimm_n = 0x04,
  nvdimm_f = 0x05,
  nvdimm_p = 0x06,
  intel_optane = 0x07,
  mrdimm = 0x08
};

enum class memory_operating_mode_capability : uint16_t {
  reserved = 0x0000,
  other = 1 << 1,
  unknown = 1 << 2,
  volatile_memory = 1 << 3,
  byte_accessible_persistent_memory = 1 << 4,
  block_accessible_persistent_memory = 1 << 5
};

memory_operating_mode_capability operator|(const memory_operating_mode_capability& lhs,
                                           const memory_operating_mode_capability& rhs);

memory_operating_mode_capability operator&(const memory_operating_mode_capability& lhs,
                                           const memory_operating_mode_capability& rhs);

struct memory_device_extended_speed {
  uint32_t value{};
  // The speed or configured memory speed of the device in MT/s.
  uint32_t speed{};

  memory_device_extended_speed& operator=(uint32_t new_value);
};

//7.18 Memory Device (Type 17)
struct memory_device : smbios {
  static constexpr auto TYPE = smbios_type::memory_device;
  //2.1+
  uint16_t physical_memory_array_handle{};
  uint16_t memory_error_info_handle{};
  uint16_t total_width{};
  uint16_t data_width{};
  uint16_t size{};
  memory_device_form_factor form_factor{};
  uint8_t device_set{};
  std::string device_locator;
  std::string bank_locator;
  memory_device_type memory_type{};
  memory_device_type_detail type_detail{};
  //2.3+
  uint16_t speed{};
  std::string manufacturer;
  std::string serial_number;
  std::string asset_tag;
  std::string part_number;
  //2.6+
  memory_device_attributes attributes{};
  //2.7+
  memory_device_extended_size extended_size{};
  uint16_t configured_memory_speed{};
  //2.8+
  uint16_t min_voltage{};
  uint16_t max_voltage{};
  uint16_t configured_voltage{};
  //3.2+
  memory_technology memory_technology{};
  memory_operating_mode_capability memory_operating_mode_capability{};
  std::string firmware_version;
  uint16_t module_manufacturer_id{};
  uint16_t module_product_id{};
  uint16_t memory_subsystem_controller_manufacturer_id{};
  uint16_t memory_subsystem_controller_product_id{};
  uint64_t non_volatile_size{};
  uint64_t volatile_size{};
  uint64_t cache_size{};
  uint64_t logical_size{};
  //3.3+
  memory_device_extended_speed extended_speed{};
  memory_device_extended_speed extended_configured_memory_speed{};
  //3.7+
  uint16_t pmic0_manufacturer_id{};
  uint16_t pmic0_revision_number{};
  uint16_t rcd_manufacturer_id{};
  uint16_t rcd_revision_number{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//Table 128 – Processor Additional Information (Type 44) structure
struct processor_additional_info : smbios {
  static constexpr auto TYPE = smbios_type::processor_additional_information;
  uint16_t referenced_handle{};
  uint8_t block_length{};
  uint8_t processor_type{};
  std::vector<uint8_t> processor_specific_data;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//Table 86 – Memory Device Mapped Address (Type 20) structure
struct memory_device_mapped_address : smbios {
  static constexpr auto TYPE = smbios_type::memory_device_mapped_address;
  //2.1+
  uint32_t starting_address{};
  uint32_t ending_address{};
  uint16_t memory_device_handle{};
  uint16_t memory_arr_mapped_addr_handle{};
  uint8_t partition_row_position{};
  uint8_t interleave_position{};
  uint8_t interleaved_data_depth{};
  uint64_t extended_starting_address{};
  uint64_t extended_ending_address{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

struct additional_info_entry : smbios {
  uint8_t entry_length{};
  uint16_t referenced_handle{};
  uint8_t referenced_offset{};
  std::string string;
  std::vector<uint8_t> value;
  std::vector<uint8_t> other;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

//Table 118 – Additional Information (Type 40) structure
struct additional_info : smbios {
  static constexpr auto TYPE = smbios_type::additional_information;
  uint8_t num_of_additional_info_entries{};
  std::vector<additional_info_entry> entries;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class system_slot_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  isa = 0x03,
  mca = 0x04,
  eisa = 0x05,
  pci = 0x06,
  pc_card_pcmcia = 0x07,
  vl_vesa = 0x08,
  proprietary = 0x09,
  processor_card_slot = 0x0A,
  proprietary_memory_card_slot = 0x0B,
  io_riser_card_slot = 0x0C,
  nubus = 0x0D,
  pci_66mhz_capable = 0x0E,
  agp = 0x0F,
  agp_2x = 0x10,
  agp_4x = 0x11,
  pci_x = 0x12,
  agp_8x = 0x13,
  m2_socket_1_dp = 0x14,
  m2_socket_1_sd = 0x15,
  m2_socket_2 = 0x16,
  m2_socket_3 = 0x17,
  mxm_type_i = 0x18,
  mxm_type_ii = 0x19,
  mxm_type_iii_standard = 0x1A,
  mxm_type_iii_he = 0x1B,
  mxm_type_iv = 0x1C,
  mxm_3_0_type_a = 0x1D,
  mxm_3_0_type_b = 0x1E,
  pci_express_gen_2_sff_8639_u2 = 0x1F,
  pci_express_gen_3_sff_8639_u2 = 0x20,
  pci_express_mini_52_pin_with_keepouts = 0x21,
  pci_express_mini_52_pin_without_keepouts = 0x22,
  pci_express_mini_76_pin = 0x23,
  pci_express_gen_4_sff_8639_u2 = 0x24,
  pci_express_gen_5_sff_8639_u2 = 0x25,
  ocp_nic_3_0_sff = 0x26,
  ocp_nic_3_0_lff = 0x27,
  ocp_nic_prior_to_3_0 = 0x28,
  cxl_flexbus_1_0 = 0x30,
  pc_98_c20 = 0xA0,
  pc_98_c24 = 0xA1,
  pc_98_e = 0xA2,
  pc_98_local_bus = 0xA3,
  pc_98_card = 0xA4,
  pci_express = 0xA5,
  pci_express_x1 = 0xA6,
  pci_express_x2 = 0xA7,
  pci_express_x4 = 0xA8,
  pci_express_x8 = 0xA9,
  pci_express_x16 = 0xAA,
  pci_express_gen_2 = 0xAB,
  pci_express_gen_2_x1 = 0xAC,
  pci_express_gen_2_x2 = 0xAD,
  pci_express_gen_2_x4 = 0xAE,
  pci_express_gen_2_x8 = 0xAF,
  pci_express_gen_2_x16 = 0xB0,
  pci_express_gen_3 = 0xB1,
  pci_express_gen_3_x1 = 0xB2,
  pci_express_gen_3_x2 = 0xB3,
  pci_express_gen_3_x4 = 0xB4,
  pci_express_gen_3_x8 = 0xB5,
  pci_express_gen_3_x16 = 0xB6,
  pci_express_gen_4 = 0xB8,
  pci_express_gen_4_x1 = 0xB9,
  pci_express_gen_4_x2 = 0xBA,
  pci_express_gen_4_x4 = 0xBB,
  pci_express_gen_4_x8 = 0xBC,
  pci_express_gen_4_x16 = 0xBD,
  pci_express_gen_5 = 0xBE,
  pci_express_gen_5_x1 = 0xBF,
  pci_express_gen_5_x2 = 0xC0,
  pci_express_gen_5_x4 = 0xC1,
  pci_express_gen_5_x8 = 0xC2,
  pci_express_gen_5_x16 = 0xC3,
  pci_express_gen_6_and_beyond = 0xC4,
  enterprise_edsff_e1 = 0xC5,
  enterprise_edsff_e3 = 0xC6
};

enum class system_slot_width : uint8_t {
  other = 0x01,
  unknown = 0x02,
  eight_bit = 0x03,
  sixteen_bit = 0x04,
  thirty_two_bit = 0x05,
  sixty_four_bit = 0x06,
  reserved = 0x07,
  one_x = 0x08,
  two_x = 0x09,
  four_x = 0x0A,
  eight_x = 0x0B,
  sixteen_x = 0x0C,
  thirty_two_x = 0x0D
};

enum class system_slot_current_usage : uint8_t {
  other = 0x01,
  unknown = 0x02,
  available = 0x03,
  in_use = 0x04,
  unavailable = 0x05,
};

enum class system_slot_length : uint8_t {
  other = 0x01,
  unknown = 0x02,
  short_length = 0x03,
  long_length = 0x04,
  drive_2_5_form_factor = 0x05,
  drive_3_5_form_factor = 0x06,
};

enum class slot_characteristics1 : uint8_t {
  characteristics_unknown = 1 << 0,
  provides_5v = 1 << 1,
  provides_3_3v = 1 << 2,
  shared_slot = 1 << 3,
  pc_support_card16 = 1 << 4,
  pc_support_cardbus = 1 << 5,
  pc_support_zoom_video = 1 << 6,
  pc_support_modem_ring_resume = 1 << 7,
};

slot_characteristics1 operator|(const slot_characteristics1& lhs, const slot_characteristics1& rhs);

slot_characteristics1 operator&(const slot_characteristics1& lhs, const slot_characteristics1& rhs);

enum class slot_characteristics2 : uint8_t {
  pci_slot_support_pme = 1 << 0,
  slot_support_hot_plug = 1 << 1,
  pci_slot_support_smbus = 1 << 2,
};

slot_characteristics2 operator|(const slot_characteristics2& lhs, const slot_characteristics2& rhs);

slot_characteristics2 operator&(const slot_characteristics2& lhs, const slot_characteristics2& rhs);

struct peer_device : smbios {
  uint16_t segment_group_number;
  uint8_t bus_number;
  uint8_t device_function_number;
  uint8_t data_bus_width;

  void read_params(bios_streambuf& istream, smbios_parser* parser) override;
};

//Table 44 – System Slots (Type 9) structure
struct system_slots : smbios {
  static constexpr auto TYPE = smbios_type::system_slots;
  //2.0+
  std::string slot_designation;
  system_slot_type slot_type{};
  system_slot_width slot_data_bus_width{};
  system_slot_current_usage current_usage{};
  system_slot_length slot_length{};
  uint16_t slot_id{};
  slot_characteristics1 slot_characteristics1{};
  //2.1+
  slot_characteristics2 slot_characteristics2{};
  //2.6+
  uint16_t segment_group_number{};
  uint8_t bus_number{};
  uint8_t device_function_number{};
  //3.2
  uint8_t data_bus_width{};
  uint8_t peer_grouping_count{};
  std::vector<peer_device> peer_groups;
  //3.4
  uint8_t slot_info{};
  uint8_t slot_physical_width{};
  uint16_t slot_pitch{};
  //3.5
  uint8_t slot_height{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class device_status : uint8_t {
  disabled = 0,
  enabled = 1,
};

enum class device_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  video = 0x03,
  scsi_controller = 0x04,
  ethernet = 0x05,
  token_ring = 0x06,
  sound = 0x07,
  pata_controller = 0x08,
  sata_controller = 0x09,
  sas_controller = 0x0A,
  wireless_lan = 0x0B,
  bluetooth = 0x0C,
  wwan = 0x0D,
  emmc = 0x0E,
  nvme = 0x0F,
  ufs_controller = 0x0F
};

struct onboard_device_type {
  uint8_t value{};
  device_status status{};
  device_type type{};

  onboard_device_type& operator=(uint8_t new_value);
};

//Table 120 – Onboard Devices Extended Information (Type 41) structure
struct onboard_devices_extended_info : smbios {
  static constexpr auto TYPE = smbios_type::onboard_devices_extended_info;
  std::string reference_designation;
  onboard_device_type device_type{};
  uint8_t device_type_instance{};
  uint16_t segment_group_number{};
  uint8_t bus_number{};
  uint8_t device_function_number{};

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};

enum class firmware_inventory_characteristics : uint16_t {
  updatable = 1 << 0,
  write_protected = 1 << 1,
};

firmware_inventory_characteristics operator|(const firmware_inventory_characteristics& lhs,
                                             const firmware_inventory_characteristics& rhs);

firmware_inventory_characteristics operator&(const firmware_inventory_characteristics& lhs,
                                             const firmware_inventory_characteristics& rhs);

//7.46 Firmware Inventory Information (Type 45)
struct firmware_inventory_info : smbios {
  static constexpr auto TYPE = smbios_type::firmware_inventory_information;
  std::string firmware_component_name;
  std::string firmware_version;
  uint8_t version_format{};
  std::string firmware_id;
  uint8_t firmware_id_format{};
  std::string release_date;
  std::string manufacturer;
  std::string lowest_supported_firmware_version;
  uint64_t image_size{};
  firmware_inventory_characteristics characteristics{};
  uint8_t state{};
  uint8_t num_of_associated_components{};
  std::vector<uint16_t> associated_component_handles;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;
};


struct unknown_struct : smbios {
  std::vector<uint8_t> data;

  void read_params(bios_streambuf& istream, smbios_parser* smbios) override;

  [[nodiscard]] std::vector<std::string> to_strings() const;
};


#endif //SMBIOS_SMBIOS_HPP
