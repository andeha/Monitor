/*
   
   PIC32MZ2064DAH169.ld -- The MIPS/PIC32 Monitor.
   
   Application with upper-half kernel link file.
   
   See PIC32MZ Graphics (DA) Family, Rev. F, Jan 2018 for details.
   
   http://ww1.microchip.com/downloads/en/DeviceDoc/60001361F.pdf
   
 */

MEMORY {
  /* PIC32MZ Graphics (DA) Family, Rev. F, Jan 2018, pp 56. */
  kseg1_boot_flash_2         : ORIGIN = 0xCFC00000, LENGTH = 0x74000
  kseg1_boot_flash_1         : ORIGIN = 0xBFC00000, LENGTH = 0x74000
  SFRs                       : ORIGIN = 0xBF800000, LENGTH = 0x100000
  kseg1_program_flash        : ORIGIN = 0xBD000000, LENGTH = 0x1FFFFF /* or 0xFFFFF (2097151 or 1048575). */
  kseg1_ddr2_sram            : ORIGIN = 0xA8000000, LENGTH = 0x1FFFFFF /* or 0xFFFFFFF. */
  kseg1_ram                  : ORIGIN = 0xA0000000, LENGTH = 0x9FFFF /* 640 kB, or 256 kB. */
  kseg0_boot_flash           : ORIGIN = 0x9FC00000, LENGTH = 0x74000
  kseg0_program_flash        : ORIGIN = 0x9D000000, LENGTH = 0x1FFFFF /* or 0xFFFFF. */
  kseg0_ddr2_sram            : ORIGIN = 0x88000000, LENGTH = 0x1FFFFFF /* or 0xFFFFFFF. */
  kseg0_ram                  : ORIGIN = 0x80000000, LENGTH = 0x9FFFF
}

Pagesize     = 4096;
__start      = 0xBFC00000; /* Ensure `start.s`, `irq.s` is the first files in `ld.lld start.s ...` */
/* __start = 0xCFC00000;  / Defined in start.s */
StackAndHeap = 4*Pagesize; /* On MZ, ≈160 pages */

SECTIONS
{
  .data   : { *(.data) }                                > kseg1_ram
  /* .bss (uninitialized data) contains global and local static vars. */
  .bss    : { _bss_begin = .; *(.bss) _bss_end = .; }   > kseg1_ram
  .reginfo_boot  : { *(.reginfo) }                      > kseg1_ram
  .abiflags_boot : { *(.MIPS.abiflags) }                > kseg1_ram
  .got ALIGN(4) : { *(.got.plt) *(.got) . = ALIGN(4) ;} > kseg1_ram
  /* Heap starts, later stack. Stack grows downwards towards heap. */
  .ahstk  : { _gp = .; . = . + StackAndHeap; _sp = .; } > kseg1_ram
  /* Stack+heap end <-- i.e stack slot 0. */
  
  /* (_main - __start) mod 4096 = 0 (bytes) */
  .text   : { start.o (.text.boot) }                    > kseg1_boot_flash_1
  .text   : { irq.o (.text.irq) }                       > kseg1_boot_flash_1
  .text   : { *(.text ) }                               > kseg1_boot_flash_1
  .rodata : { *(.rodata) }                              > kseg1_boot_flash_1
}