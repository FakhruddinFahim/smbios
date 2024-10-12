//
// Created by Fakhr on 24/05/2024.
//

#ifndef WINT_SMBIOS_H
#define WINT_SMBIOS_H

#include <string>
#include <cinttypes>
#include <vector>
#include <streambuf>
#include <istream>
#include <cstring>
#include "bios_streambuf.hpp"

//Version
enum smbios_version : uint32_t {
  SMBIOS_2_0 = 0x0200,
  SMBIOS_2_1 = 0x0201,
  SMBIOS_2_2 = 0x0202,
  SMBIOS_2_3 = 0x0203,
  SMBIOS_2_4 = 0x0204,
  SMBIOS_2_5 = 0x0205,
  SMBIOS_2_6 = 0x0206,
  SMBIOS_2_7 = 0x0207,
  SMBIOS_2_8 = 0x0208,
  SMBIOS_3_0 = 0x0300,
  SMBIOS_3_1 = 0x0301,
  SMBIOS_3_2 = 0x0302,
  SMBIOS_3_3 = 0x0303,
  SMBIOS_3_4 = 0x0304,
  SMBIOS_3_5 = 0x0305,
  SMBIOS_3_7 = 0x0307,
};

struct smbios_header {
  uint8_t type{};
  uint8_t length{};
  uint16_t handle{};

  void read(bios_streambuf& istream);
};

struct smbios_ctx;

struct smbios {
  smbios_header header;

  void read(bios_streambuf& istream, smbios_ctx* smbios_ctx);

  virtual void read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) = 0;

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


  void read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) override;
};

//SMBIOS 3.0 (64-bit) Entry Point structure
struct smbios_entry_point64 : public smbios_entry_point {
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


  void read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) override;

};

struct smbios_ctx {
  uint32_t version = 0;
  smbios_entry_point* entry_point = nullptr;
  std::vector<smbios*> structures;
  std::vector<uint8_t> buffer;

  void parse(std::vector<uint8_t>& buffer);

  std::string sys_product_name();

  std::string sys_uuid();

  ~smbios_ctx();
};


//7.1 BIOS Information (Type 0)
enum class bios_characteristics : uint64_t {
  reserved_0 = 0x0000000000000001, // Bit 0: Reserved
  reserved_1 = 0x0000000000000002, // Bit 1: Reserved
  unknown = 0x0000000000000004, // Bit 2: Unknown
  not_supported = 0x0000000000000008, // Bit 3: BIOS Characteristics are not supported
  isa_supported = 0x0000000000000010, // Bit 4: ISA is supported
  mca_supported = 0x0000000000000020, // Bit 5: MCA is supported
  eisa_supported = 0x0000000000000040, // Bit 6: EISA is supported
  pci_supported = 0x0000000000000080, // Bit 7: PCI is supported
  pc_card_supported = 0x0000000000000100, // Bit 8: PC card (PCMCIA) is supported
  plug_and_play_supported = 0x0000000000000200, // Bit 9: Plug and Play is supported
  apm_supported = 0x0000000000000400, // Bit 10: APM is supported
  bios_upgradeable = 0x0000000000000800, // Bit 11: BIOS is upgradeable (Flash)
  bios_shadowing_allowed = 0x0000000000001000, // Bit 12: BIOS shadowing is allowed
  vl_vesa_supported = 0x0000000000002000, // Bit 13: VL-VESA is supported
  escd_supported = 0x0000000000004000, // Bit 14: ESCD support is available
  boot_from_cd_supported = 0x0000000000008000, // Bit 15: Boot from CD is supported
  selectable_boot_supported = 0x0000000000010000, // Bit 16: Selectable boot is supported
  bios_rom_socketed = 0x0000000000020000, // Bit 17: BIOS ROM is socketed
  boot_from_pc_card_supported = 0x0000000000040000, // Bit 18: Boot from PC card (PCMCIA) is supported
  edd_specification_supported = 0x0000000000080000, // Bit 19: EDD specification is supported
  japanese_floppy_nec_9800_supported = 0x0000000000100000,
  // Bit 20: Int 13h — Japanese floppy for NEC 9800 1.2 MB (3.5”, 1K bytes/sector, 360 RPM) is supported
  japanese_floppy_toshiba_supported = 0x0000000000200000,
  // Bit 21: Int 13h — Japanese floppy for Toshiba 1.2 MB (3.5”, 360 RPM) is supported
  floppy_5_25_360kb_supported = 0x0000000000400000, // Bit 22: Int 13h — 5.25” / 360 KB floppy services are supported
  floppy_5_25_1_2mb_supported = 0x0000000000800000, // Bit 23: Int 13h — 5.25” / 1.2 MB floppy services are supported
  floppy_3_5_720kb_supported = 0x0000000001000000, // Bit 24: Int 13h — 3.5” / 720 KB floppy services are supported
  floppy_3_5_2_88mb_supported = 0x0000000002000000, // Bit 25: Int 13h — 3.5” / 2.88 MB floppy services are supported
  print_screen_service_supported = 0x0000000004000000, // Bit 26: Int 5h, print screen service is supported
  keyboard_service_supported = 0x0000000008000000, // Bit 27: Int 9h, 8042 keyboard services are supported
  serial_service_supported = 0x0000000010000000, // Bit 28: Int 14h, serial services are supported
  printer_service_supported = 0x0000000020000000, // Bit 29: Int 17h, printer services are supported
  video_service_supported = 0x0000000040000000, // Bit 30: Int 10h, CGA/Mono video services are supported
  nec_pc_98 = 0x0000000080000000, // Bit 31: NEC PC-98
  reserved_for_bios_vendor = 0x0000FFFF00000000, // Bits 32:47: Reserved for BIOS vendor
  reserved_for_system_vendor = 0xFFFF000000000000 // Bits 48:63: Reserved for system vendor
};

enum class bios_char_ext_byte1 : uint8_t {
  acpi_supported = 0x01, // Bit 0
  usb_legacy_supported = 0x02, // Bit 1
  agp_supported = 0x04, // Bit 2
  i2o_boot_supported = 0x08, // Bit 3
  ls_120_boot_supported = 0x10, // Bit 4
  atapi_zip_boot_supported = 0x20, // Bit 5
  ieee_1394_boot_supported = 0x40, // Bit 6
  smart_battery_supported = 0x80 // Bit 7
};

enum class bios_char_ext_byte2 : uint8_t {
  bios_boot_specification_supported = 0x01, // Bit 0
  function_key_network_service_boot_supported = 0x02, // Bit 1
  targeted_content_distribution_enabled = 0x04, // Bit 2
  uefi_specification_supported = 0x08, // Bit 3
  smbios_describes_virtual_machine = 0x10, // Bit 4
  reserved = 0xE0 // Bits 5:7 reserved for future assignment
};

struct bios_info : public smbios {
  const static uint8_t TYPE = 0;
  //version 2.0+
  std::string vendor{};
  std::string bios_version{};
  uint16_t bios_starting_addr_seg{};
  std::string bios_release_date{};
  uint8_t bios_rom_size{}; //kb = (bios_rom_size + 1) * 64
  uint64_t bios_characteristics{};
  //version 2.4+
  std::vector<uint8_t> bios_characteristics_ext_bytes;
  uint8_t system_bios_major_release{};
  uint8_t system_bios_minor_release{};
  uint8_t embedded_controller_firmware_major_release{};
  uint8_t embedded_controller_firmware_minor_release{};
  //version 3.1+
  uint16_t extended_bios_rom_size{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) override;
};


//System Information (Type 1) structure
enum class wake_up_type : uint8_t {
  reserved = 0x00, // 00h: Reserved
  other = 0x01, // 01h: Other
  unknown = 0x02, // 02h: Unknown
  apm_timer = 0x03, // 03h: APM Timer
  modem_ring = 0x04, // 04h: Modem Ring
  lan_remote = 0x05, // 05h: LAN Remote
  power_switch = 0x06, // 06h: Power Switch
  pci_pme = 0x07, // 07h: PCI PME#
  ac_power_restored = 0x08 // 08h: AC Power Restored
};

struct sys_info : public smbios {
  const static uint8_t TYPE = 1;
  //version 2.0+
  std::string manufacturer;
  std::string product_name;
  std::string version;
  std::string serial_number;
  //version 2.1+
  uint8_t uuid[16]{};
  uint8_t wake_up_type{};
  //version 2.4+
  std::string sku_number;
  std::string family;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


enum class feature_flag : uint8_t {
  hosting_board = 0x01, // Bit 0: Set to 1 if the board is a hosting board
  requires_auxiliary_cards = 0x02,
  // Bit 1: Set to 1 if the board requires at least one daughter board or auxiliary card to function properly
  removable_board = 0x04, // Bit 2: Set to 1 if the board is removable
  replaceable_board = 0x08, // Bit 3: Set to 1 if the board is replaceable
  hot_swappable_board = 0x10, // Bit 4: Set to 1 if the board is hot swappable
  reserved = 0xE0 // Bits 5:7: Reserved for future definition by this specification; set to 000b
};

enum class board_type : uint8_t {
  unknown = 0x01, // Byte Value 01h: Unknown
  other = 0x02, // Byte Value 02h: Other
  server_blade = 0x03, // Byte Value 03h: Server Blade
  connectivity_switch = 0x04, // Byte Value 04h: Connectivity Switch
  system_management_module = 0x05, // Byte Value 05h: System Management Module
  processor_module = 0x06, // Byte Value 06h: Processor Module
  io_module = 0x07, // Byte Value 07h: I/O Module
  memory_module = 0x08, // Byte Value 08h: Memory Module
  daughter_board = 0x09, // Byte Value 09h: Daughter board
  motherboard = 0x0A, // Byte Value 0Ah: Motherboard (includes processor, memory, and I/O)
  processor_memory_module = 0x0B, // Byte Value 0Bh: Processor/Memory Module
  processor_io_module = 0x0C, // Byte Value 0Ch: Processor/IO Module
  interconnect_board = 0x0D // Byte Value 0Dh: Interconnect board
};

//Baseboard (or Module) Information (Type 2)
struct baseboard_info : public smbios {
  const static uint8_t TYPE = 2;
  std::string manufacturer;
  std::string product;
  std::string version;
  std::string serial_number;
  std::string asset_tag;
  uint8_t feature_flags{};
  std::string location_in_chassis;
  uint16_t chassis_handle{};
  uint8_t board_type{};
  uint8_t num_of_contained_object_handles{};
  std::vector<uint16_t> contained_object_handles;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

enum class chassis_type : uint8_t {
  other = 0x01, // 01h: Other
  unknown = 0x02, // 02h: Unknown
  desktop = 0x03, // 03h: Desktop
  low_profile_desktop = 0x04, // 04h: Low Profile Desktop
  pizza_box = 0x05, // 05h: Pizza Box
  mini_tower = 0x06, // 06h: Mini Tower
  tower = 0x07, // 07h: Tower
  portable = 0x08, // 08h: Portable
  laptop = 0x09, // 09h: Laptop
  notebook = 0x0A, // 0Ah: Notebook
  hand_held = 0x0B, // 0Bh: Hand Held
  docking_station = 0x0C, // 0Ch: Docking Station
  all_in_one = 0x0D, // 0Dh: All in One
  sub_notebook = 0x0E, // 0Eh: Sub Notebook
  space_saving = 0x0F, // 0Fh: Space-saving
  lunch_box = 0x10, // 10h: Lunch Box
  main_server_chassis = 0x11, // 11h: Main Server Chassis
  expansion_chassis = 0x12, // 12h: Expansion Chassis
  sub_chassis = 0x13, // 13h: SubChassis
  bus_expansion_chassis = 0x14, // 14h: Bus Expansion Chassis
  peripheral_chassis = 0x15, // 15h: Peripheral Chassis
  raid_chassis = 0x16, // 16h: RAID Chassis
  rack_mount_chassis = 0x17, // 17h: Rack Mount Chassis
  sealed_case_pc = 0x18, // 18h: Sealed-case PC
  multi_system_chassis = 0x19, // 19h: Multi-system chassis
  compact_pci = 0x1A, // 1Ah: Compact PCI
  advanced_tca = 0x1B, // 1Bh: Advanced TCA
  blade = 0x1C, // 1Ch: Blade
  blade_enclosure = 0x1D, // 1Dh: Blade Enclosure
  tablet = 0x1E, // 1Eh: Tablet
  convertible = 0x1F, // 1Fh: Convertible
  detachable = 0x20, // 20h: Detachable
  iot_gateway = 0x21, // 21h: IoT Gateway
  embedded_pc = 0x22, // 22h: Embedded PC
  mini_pc = 0x23, // 23h: Mini PC
  stick_pc = 0x24 // 24h: Stick PC
};

enum class boot_up_state : uint8_t {
  other = 0x01, // 01h: Other
  unknown = 0x02, // 02h: Unknown
  safe = 0x03, // 03h: Safe
  warning = 0x04, // 04h: Warning
  critical = 0x05, // 05h: Critical
  non_recoverable = 0x06 // 06h: Non-recoverable
};

enum class security_status : uint8_t {
  other = 0x01, // 01h: Other
  unknown = 0x02, // 02h: Unknown
  none = 0x03, // 03h: None
  external_interface_locked_out = 0x04, // 04h: External interface locked out
  external_interface_enabled = 0x05 // 05h: External interface enabled
};

//System Enclosure or Chassis (Type 3) structure
struct sys_enclosure : public smbios {
  const static uint8_t TYPE = 3;
  //2.0+
  std::string manufacturer;
  uint8_t type{};
  std::string version;
  std::string serial_number;
  std::string asset_tag_number;
  //2.1+
  uint8_t boot_up_state{};
  uint8_t power_supply_state{};
  uint8_t thermal_state{};
  uint8_t security_status{};
  //2.3+
  uint32_t oem_defined{};
  uint8_t height{};
  uint8_t num_of_power_cords{};
  uint8_t contained_element_count{};
  uint8_t contained_element_record_length{};
  std::vector<uint8_t> contained_elements;
  //2.7+
  std::string sku_number;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

enum class processor_type : uint8_t {
  other = 0x01, // 01h: Other
  unknown = 0x02, // 02h: Unknown
  central_processor = 0x03, // 03h: Central Processor
  math_processor = 0x04, // 04h: Math Processor
  dsp_processor = 0x05, // 05h: DSP Processor
  video_processor = 0x06 // 06h: Video Processor
};

enum class processor_family : uint16_t {
  other = 0x0001, // 01h: Other
  unknown = 0x0002, // 02h: Unknown
  _8086 = 0x0003, // 03h: 8086
  _80286 = 0x0004, // 04h: 80286
  intel_386_processor = 0x0005, // 05h: Intel386™ processor
  intel_486_processor = 0x0006, // 06h: Intel486™ processor
  _8087 = 0x0007, // 07h: 8087
  _80287 = 0x0008, // 08h: 80287
  _80387 = 0x0009, // 09h: 80387
  _80487 = 0x000A, // 0Ah: 80487
  intel_pentium_processor = 0x000B, // 0Bh: Intel® Pentium® processor
  pentium_pro_processor = 0x000C, // 0Ch: Pentium® Pro processor
  pentium_ii_processor = 0x000D, // 0Dh: Pentium® II processor
  pentium_processor_with_mmx_technology = 0x000E, // 0Eh: Pentium® processor with MMX™ technology
  intel_celeron_processor = 0x000F, // 0Fh: Intel® Celeron® processor
  pentium_ii_xeon_processor = 0x0010, // 10h: Pentium® II Xeon™ processor
  pentium_iii_processor = 0x0011, // 11h: Pentium® III processor
  m1_family = 0x0012, // 12h: M1 Family
  m2_family = 0x0013, // 13h: M2 Family
  intel_celeron_m_processor = 0x0014, // 14h: Intel® Celeron® M processor
  intel_pentium_4_ht_processor = 0x0015, // 15h: Intel® Pentium® 4 HT processor
  pentium_ii_xeon_processor_ = 0x0016, // 16h: Pentium® II Xeon™ processor
  pentium_iii_processor_ = 0x0017, // 17h: Pentium® III processor
  intel_atom_processor = 0x0018, // 18h: Intel® Atom™ processor
  intel_core_m_processor = 0x0019, // 19h: Intel® Core™ M processor
  intel_core_m3_processor = 0x001A, // 1Ah: Intel(R) Core(TM) m3 processor
  intel_core_m5_processor = 0x001B, // 1Bh: Intel(R) Core(TM) m5 processor
  intel_core_m7_processor = 0x001C, // 1Ch: Intel(R) Core(TM) m7 processor
  alpha_family = 0x001D, // 1Dh: Alpha Family
  alpha_21064 = 0x001E, // 1Eh: Alpha 21064
  alpha_21066 = 0x001F, // 1Fh: Alpha 21066
  alpha_21164 = 0x0020, // 20h: Alpha 21164
  alpha_21164pc = 0x0021, // 21h: Alpha 21164PC
  alpha_21164a = 0x0022, // 22h: Alpha 21164a
  alpha_21264 = 0x0023, // 23h: Alpha 21264
  alpha_21364 = 0x0024, // 24h: Alpha 21364
  amd_turion_ii_ultra_dual_core_mobile_processor_family = 0x0025,
  // 25h: AMD Turion™ II Ultra Dual-Core Mobile M Processor Family
  amd_turion_ii_dual_core_mobile_processor_family = 0x0026, // 26h: AMD Turion™ II Dual-Core Mobile M Processor Family
  amd_athlon_ii_dual_core_m_processor_family = 0x0027, // 27h: AMD Athlon™ II Dual-Core M Processor Family
  amd_opteron_6100_series_processor = 0x0028, // 28h: AMD Opteron™ 6100 Series Processor
  amd_opteron_4100_series_processor = 0x0029, // 29h: AMD Opteron™ 4100 Series Processor
  amd_opteron_6200_series_processor = 0x002A, // 2Ah: AMD Opteron™ 6200 Series Processor
  amd_opteron_4200_series_processor = 0x002B, // 2Bh: AMD Opteron™ 4200 Series Processor
  amd_fx_series_processor = 0x002C, // 2Ch: AMD FX™ Series Processor
  mips_family = 0x002D, // 2Dh: MIPS Family
  mips_r4000 = 0x002E, // 2Eh: MIPS R4000
  mips_r4200 = 0x002F, // 2Fh: MIPS R4200
  mips_r4400 = 0x0030, // 30h: MIPS R4400
  mips_r4600 = 0x0031, // 31h: MIPS R4600
  mips_r10000 = 0x0032, // 32h: MIPS R10000
  amd_c_series_processor = 0x0033, // 33h: AMD C-Series Processor
  amd_e_series_processor = 0x0034, // 34h: AMD E-Series Processor
  amd_a_series_processor = 0x0035, // 35h: AMD A-Series Processor
  amd_g_series_processor = 0x0036, // 36h: AMD G-Series Processor
  amd_z_series_processor = 0x0037, // 37h: AMD Z-Series Processor
  amd_r_series_processor = 0x0038, // 38h: AMD R-Series Processor
  amd_opteron_4300_series_processor = 0x0039, // 39h: AMD Opteron™ 4300 Series Processor
  amd_opteron_6300_series_processor = 0x003A, // 3Ah: AMD Opteron™ 6300 Series Processor
  amd_opteron_3300_series_processor = 0x003B, // 3Bh: AMD Opteron™ 3300 Series Processor
  amd_firepro_series_processor = 0x003C, // 3Ch: AMD FirePro™ Series Processor
  sparc_family = 0x003D, // 3Dh: SPARC Family
  supersparc = 0x003E, // 3Eh: SuperSPARC
  microsparc_ii = 0x003F, // 3Fh: microSPARC II
  microsparc_iiep = 0x0040, // 40h: microSPARC IIep
  ultrasparc = 0x0041, // 41h: UltraSPARC
  ultrasparc_ii = 0x0042, // 42h: UltraSPARC II
  ultrasparc_iii = 0x0043, // 43h: UltraSPARC Iii
  ultrasparc_iiii = 0x0044, // 44h: UltraSPARC III
  ultrasparc_iiiii = 0x0045, // 45h: UltraSPARC IIIi
  amd_athlon_x4_quad_core_processor_family = 0x0046, // 46h: AMD Athlon(TM) X4 Quad-Core Processor Family
  amd_opteron_x1000_series_processor = 0x0047, // 47h: AMD Opteron(TM) X1000 Series Processor
  amd_opteron_x2000_series_apu = 0x0048, // 48h: AMD Opteron(TM) X2000 Series APU
  amd_opteron_a_series_processor = 0x0049, // 49h: AMD Opteron(TM) A-Series Processor
  amd_opteron_x3000_series_apu = 0x004A, // 4Ah: AMD Opteron(TM) X3000 Series APU
  amd_zen_processor_family = 0x004B, // 4Bh: AMD Zen Processor Family
  hobbit_family = 0x004C, // 4Ch: Hobbit Family
  crusoe_tm5000_family = 0x004D, // 4Dh: Crusoe™ TM5000 Family
  crusoe_tm3000_family = 0x004E, // 4Eh: Crusoe™ TM3000 Family
  efficeon_tm8000_family = 0x004F, // 4Fh: Efficeon™ TM8000 Family
  via_c7_m_processor_family = 0x0050, // 50h: VIA C7™-M Processor Family
  via_c7_d_processor_family = 0x0051, // 51h: VIA C7™-D Processor Family
  via_c7_processor_family = 0x0052, // 52h: VIA C7™ Processor Family
  via_eden_processor_family = 0x0053, // 53h: VIA Eden™ Processor Family
  multi_core_intel_xeon_processor = 0x0054, // 54h: Multi-Core Intel® Xeon® processor
  dual_core_intel_xeon_processor_3xxx_series = 0x0055, // 55h: Dual-Core Intel® Xeon® processor 3xxx Series
  quad_core_intel_xeon_processor_3xxx_series = 0x0056, // 56h: Quad-Core Intel® Xeon® processor 3xxx Series
  via_nano_processor_family = 0x0057, // 57h: VIA Nano™ Processor Family
  dual_core_intel_xeon_processor_5xxx_series = 0x0058, //
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
  bit64capable = 1 << 2,
  multicore = 1 << 3,
  hardwarethread = 1 << 4,
  executeprotection = 1 << 5,
  enhancedvirtualization = 1 << 6,
  powerperformancecontrol = 1 << 7,
  reserved2 = 0xFF00,
};

//Processor Information (Type 4)
struct processor_info : public smbios {
  const static uint8_t TYPE = 4;
  //2.0+
  std::string socket_designation;
  uint8_t processor_type{};
  uint8_t processor_family{};
  std::string processor_manufacturer;
  uint64_t processor_id{};
  std::string processor_version;
  uint8_t voltage{};
  uint16_t external_clock{};
  uint16_t max_speed{};
  uint16_t current_speed{};
  uint8_t status{};
  uint8_t processor_upgrade{};
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
  uint16_t processor_characteristics{};
  //2.6+
  uint16_t processor_family_2{};
  //3.0+
  uint16_t core_count_2{};
  uint16_t core_enabled_2{};
  uint16_t thread_count_2{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

enum class sram_type : uint16_t {
  other = 1 << 0,
  unknown = 1 << 1,
  non_burst = 1 << 2,
  burst = 1 << 3,
  pipeline_burst = 1 << 4,
  synchronous = 1 << 5,
  asynchronous = 1 << 6,
  reserved = 0xFF80 // Bits 7 to 15 are reserved
};

enum class error_correction_type : uint8_t {
  other = 0x01,
  unknown = 0x02,
  none = 0x03,
  parity = 0x04,
  single_bit_ecc = 0x05,
  multi_bit_ecc = 0x06
};

enum class sys_cache_type : uint8_t {
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
  _2_way_set_associative = 0x04,
  _4_way_set_associative = 0x05,
  fully_associative = 0x06,
  _8_way_set_associative = 0x07,
  _16_way_set_associative = 0x08,
  _12_way_set_associative = 0x09,
  _24_way_set_associative = 0x0A,
  _32_way_set_associative = 0x0B,
  _48_way_set_associative = 0x0C,
  _64_way_set_associative = 0x0D,
  _20_way_set_associative = 0x0E
};

//7.8 Cache Information (Type 7)
struct cache_info : public smbios {
  const static uint8_t TYPE = 7;
  //2.0+
  std::string socket_designation;
  uint16_t cache_configuration{};
  uint16_t max_cache_size{};
  uint16_t installed_size{};
  uint16_t supported_sram_type{};
  uint16_t current_sram_type{};
  //2.1+
  uint8_t cache_speed{};
  uint8_t error_correction_type{};
  uint8_t system_cache_type{};
  uint8_t associativity{};
  //3.1+
  uint32_t max_cache_size2{};
  uint32_t installed_cache_size2{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.9 Port Connector Information (Type 8)
struct port_connector_info : public smbios {
  const static uint8_t TYPE = 8;
  std::string internal_ref_designator;
  uint8_t internal_connector_type{};
  std::string external_ref_designator;
  uint8_t external_connector_type{};
  uint8_t port_type{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.12 OEM Strings (Type 11)
struct oem_strings : public smbios {
  const static uint8_t TYPE = 11;
  std::vector<std::string> strings;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


struct sys_configuration_options : public smbios {
  const static uint8_t TYPE = 12;
  std::vector<std::string> strings;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


//7.17 Physical Memory Array (Type 16)
struct physical_memory_arr : public smbios {
  const static uint8_t TYPE = 16;
  //2.1+
  uint8_t location{};
  uint8_t use{};
  uint8_t memory_err_correction{};
  uint32_t max_capacity{};
  uint16_t memory_err_info_handle{};
  uint16_t num_of_memory_device{};
  //2.7+
  uint64_t extended_max_capacity{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

enum class memory_err_type : uint8_t {
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

enum class memory_err_granularity : uint8_t {
  other = 0x01,
  unknown = 0x02,
  device_level = 0x03,
  memory_partition_level = 0x04,
};

enum class memory_err_operation : uint8_t {
  other = 0x01,
  unknown = 0x02,
  read = 0x03,
  write = 0x04,
  partial_write = 0x05,
};

//7.19 32-Bit Memory Error Information (Type 18)
struct memory_err_info : public smbios {
  const static uint8_t TYPE = 18;
  //2.1+
  uint8_t err_type{};
  uint8_t err_granularity{};
  uint8_t err_operation{};
  uint32_t vendor_syndrome{};
  uint32_t memory_arr_err_addr{};
  uint32_t device_err_addr{};
  uint32_t err_resolution{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.20 Memory Array Mapped Address (Type 19)
struct memory_arr_mapped_addr : public smbios {
  const static uint8_t TYPE = 19;
  //2.1+
  uint32_t starting_addr{};
  uint32_t ending_addr{};
  uint16_t memory_arr_handle{};
  uint8_t partition_width{};
  //2.7+
  uint64_t extended_starting_addr{};
  uint64_t extended_ending_addr{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.33 System Boot Information (Type 32)
struct sys_boot_info : public smbios {
  const static uint8_t TYPE = 32;
  uint8_t reserved[6]{};
  uint8_t boot_status[10]{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


//7.44 TPM Device (Type 43)
struct tpm_device : public smbios {
  const static uint8_t TYPE = 43;
  uint8_t vendor_id[4]{};
  uint8_t major_spec_version{};
  uint8_t minor_spec_version{};
  uint32_t firmware_version1{};
  uint32_t firmware_version2{};
  std::vector<std::string> description;
  uint64_t characteristics{};
  uint32_t oem_defined{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.18 Memory Device (Type 17)
struct memory_device : public smbios {
  const static uint8_t TYPE = 17;
  //2.1+
  uint16_t physical_memory_arr_handle{};
  uint16_t memory_err_info_handle{};
  uint16_t total_width{};
  uint16_t data_width{};
  uint16_t size{};
  uint8_t form_factor{};
  uint8_t device_set{};
  std::string device_locator;
  std::string bank_locator;
  uint8_t memory_type{};
  uint16_t type_detail{};
  //2.3+
  uint16_t speed{};
  std::string manufacturer;
  std::string serial_number;
  std::string asset_tag;
  std::string part_number;
  //2.6+
  uint8_t attributes{};
  //2.7+
  uint32_t extended_size{};
  uint16_t configured_memory_speed{};
  //2.8+
  uint16_t min_voltage{};
  uint16_t max_voltage{};
  uint16_t configured_voltage{};
  //3.2+
  uint8_t memory_technology{};
  uint16_t memory_operating_mode_capability{};
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
  uint32_t extended_speed{};
  uint32_t extended_configured_memory_speed{};
  //3.7+
  uint16_t pmic0_manufacturer_id{};
  uint16_t pmic0_revision_number{};
  uint16_t rcd_manufacturer_id{};
  uint16_t rcd_revision_number{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//Table 128 – Processor Additional Information (Type 44) structure
struct processor_additional_info : public smbios {
  const static uint8_t TYPE = 44;
  uint16_t referenced_handle{};
  uint8_t block_length{};
  uint8_t processor_type{};
  std::vector<uint8_t> processor_specific_data;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//Table 86 – Memory Device Mapped Address (Type 20) structure
struct memory_device_mapped_address : public smbios {
  const static uint8_t TYPE = 20;
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

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


struct additional_info_entry : public smbios {
  uint8_t entry_length{};
  uint16_t referenced_handle{};
  uint8_t referenced_offset{};
  std::string string;
  std::vector<uint8_t> value;
  std::vector<uint8_t> other;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//Table 118 – Additional Information (Type 40) structure
struct additional_info : public smbios {
  const static uint8_t TYPE = 40;
  uint8_t num_of_additional_info_entries{};
  std::vector<additional_info_entry> entries;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

struct peer_device : public smbios {
  uint16_t segment_group_number;
  uint8_t bus_number;
  uint8_t device_function_number;
  uint8_t data_bus_width;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios_ctx) override;
};

//Table 44 – System Slots (Type 9) structure
struct system_slot : public smbios {
  const static uint8_t TYPE = 9;
  //2.0+
  std::string slot_designation;
  uint8_t slot_type{};
  uint8_t slot_data_bus_width{};
  uint8_t current_usage{};
  uint8_t slot_length{};
  uint16_t slot_id{};
  uint8_t slot_characteristics1{};
  //2.1+
  uint8_t slot_characteristics2{};
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

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//Table 120 – Onboard Devices Extended Information (Type 41) structure
struct onboard_devices_extended_info : public smbios {
  const static uint8_t TYPE = 41;
  std::string reference_designation;
  uint8_t device_type{};
  uint8_t device_type_instance{};
  uint16_t segment_group_number{};
  uint8_t bus_number{};
  uint8_t device_function_number{};

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};

//7.46 Firmware Inventory Information (Type 45)
struct firmware_inventory_info : public smbios {
  const static uint8_t TYPE = 45;
  std::string firmware_component_name;
  std::string firmware_version;
  uint8_t version_format{};
  std::string firmware_id;
  uint8_t firmware_id_format{};
  std::string release_date;
  std::string manufacturer;
  std::string lowest_supported_firmware_version;
  uint64_t image_size{};
  uint16_t characteristics{};
  uint8_t state{};
  uint8_t num_of_associated_components{};
  std::vector<uint16_t> associated_component_handles;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;
};


struct unknown_struct : public smbios {
  std::vector<uint8_t> data;

  void read_params(bios_streambuf& istream, smbios_ctx* smbios) override;

  std::vector<std::string> to_strings();
};

smbios_ctx parse_smbios();

#endif //WINT_SMBIOS_H
