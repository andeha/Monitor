/*
   
   PIC32MX795F512L.ld -- The MIPS/PIC32 Monitor.
   
   Upper-half kernel link file. Apps are stored in `useg`.
   
   See PIC32MX5XX/6XX/7XX Family Data Sheets, Rev. H, March 2013 for details.
   
   http://ww1.microchip.com/downloads/en/DeviceDoc/60001156J.pdf
   
 */

MEMORY {
  /* PIC32MX5XX/6XX/7XX Family Data Sheets, Rev. H, March 2013, pp 53. */
  Config3                    : ORIGIN = 0xBFC02FF0, LENGTH = 0x4
  Config2                    : ORIGIN = 0xBFC02FF4, LENGTH = 0x4
  Config1                    : ORIGIN = 0xBFC02FF8, LENGTH = 0x4
  Config0                    : ORIGIN = 0xBFC02FFC, LENGTH = 0x4
  /* Non-cachable. I.e the prefetch cache is *disabled*. Use kseg0 to improve speed. */
  kseg1_boot_flash           : ORIGIN = 0xBFC00000, LENGTH = 0x2FF0   /* 12 271 bytes. On MZDA: 475 135 bytes. */
  SFRs                       : ORIGIN = 0xBF800000, LENGTH = 0x100000
  kseg1_program_flash        : ORIGIN = 0xBD000000, LENGTH = 0x80000  /* 524287, 262183, 131071 or 65536 bytes. On MZDA: 2 or 1 MB. */
  kseg1_ram            (w!x) : ORIGIN = 0xA0000000, LENGTH = 0x20000  /* 32768, 16384, 65536 or 131071 bytes. On MZDA: 640 kB, or 256 kB. */
  /* Cachable */
  kseg0_boot_flash           : ORIGIN = 0x9FC00000, LENGTH = 0x2FF0   /* See `kseg1_boot_flash`. */
  kseg0_program_flash   (rx) : ORIGIN = 0x9D000000, LENGTH = 0x80000  /* See `kseg1_program_flash`. */
  kseg0_ram                  : ORIGIN = 0x80000000, LENGTH = 0x20000  /* See `kseg1_ram`. */
}

Pagesize     = 4096;
__start      = 0xBFC00000; /* Ensure `start.s`, `irq.s` is the first files in `ld.lld start.s ...` */
StackAndHeap = 4*Pagesize; /* On MZ, ≈160 pages */

SECTIONS
{
  /* Appl. Code     00000000 7FFFFFFF   512 MB User mapped            (useg) */
  /* Kernel Code    80000000 9FFFFFFF   512 MB Kernel unmapped       (kseg0) */
  /* Kernel Data 	a0000000 bfffffff   512 MB Kernel unmapped and uncached, 
                                        i.e can be used with DMA     (kseg1) */
  /* Kernel R/W     e0000000 ffffffff   512 MB Kernel mapped ∧ cached, that  
                                        can be swapped out on disk   (kseg3) */
  
  /* Modifiable global/static variables with pre-defined values are stored in 
     .data. Below, sections are to the left and segments to the right. */
  
  .data   : { *(.data) }                                > kseg1_ram
  /* .bss (uninitialized data) contains global and local static vars. */
  .bss    : { _bss_begin = .; *(.bss) _bss_end = .; }   > kseg1_ram
  .reginfo_boot  : { *(.reginfo) }                      > kseg1_ram
  .abiflags_boot : { *(.MIPS.abiflags) }                > kseg1_ram
  /* Heap starts, later stack. Stack grows downwards towards heap. */
  .ahstk  : { _gp = .; . = . + StackAndHeap; _sp = .; } > kseg1_ram
  /* Stack+heap end <-- i.e stack slot 0. */
  
  /* (_main - __start) mod 4096 = 0 (bytes) */
  .text   : { start.o (.text.boot) }                    > kseg1_boot_flash
  .text   : { irq.o (.text.irq) }                       > kseg1_boot_flash
  .text   : { *(.text ) }                               > kseg1_boot_flash
  .rodata : { *(.rodata) }                              > kseg1_boot_flash
  
}
