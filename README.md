# A MIPS/PIC32 Monitor Program

**This program helps you** upload programs, interact with a chip, find errors in your program and understand how control flows when a program is running.

[**monitor_PIC32MX470F512L.hex**][monitor_0a1f92] Enables the chips' UART5 where port RF13 (pin 39) is U5TX, and port RF12 (pin 40) is U5RX. This binary can upload programs up to 64 kB of size. 

[**nomonitor_MX.hex**][nomonitor_0a1f92] The same startup code as the application above, but without any monitor capabilities. Suitable for concatenation with your production code. 

[monitor_0a1f92]: https://github.com/andeha/Monitor/blob/master/Releases/monitor_PIC32MX795F512L_0ab1f92.hex
[nomonitor_0a1f92]: https://github.com/andeha/Monitor/blob/master/Releases/nomonitor_MX_0ab1f92.hex

## Connecting the Chip to a PC or a Mac 

To interact with a USB-port equipped PIC32-board via a USB cable connected to your computer, do the following:
	
On a **Mac**, start a VT100 terminal emulator with something similar to:

    prompt% screen /dev/cu.usbserial-A506K4XV 600

On a **PC**, use hyperterm or putty.

## Exploring the PIC32 Using Monitor.hex

To view the current value in a memory cell (`peek`), type 

    Welcome to your MIPS/PIC32 Monitor
    ...
    ? 0x12341234
    Address 0x12341234 contains 0x00000001 

To alter the value in a memory cell (`poke`), write

    ! 0x12341234 0x00000002
    Poke 0x12341234 from 0x00000001 to 0x00000002

To toggle tracing, use
    
`\` Disable tracing (default)<br/>
`|` Enable assembly trace<br/>
`/` Enable source trace<br/>
    
To interactively add a breakpoint, enter `* 0x12341234`.

To single-step, enter `s`.

To continue execution after a breakpoint is hit, enter `c`.

To display a snapshot of the values in the peripheral registers, write: `~`.

## Loading Programs

To load software onto the PIC32, copy its [Intel Hex](https://en.wikipedia.org/wiki/Intel_HEX) file into the paste buffer and paste the content into your `screen` session with `C-a` followed by `]` as in:

    terminal$ pbcopy < mykernel.hex
    terminal$ screen /dev/cu.usbserial-A506K4XV 600
        Press C-a ] to load the program and C-a c to exit.        

To capture your `screen` session on file, press `C-a` and `>`.

## About Startup

The startup code in **monitor** and **nomonitor** resides starting at 0xBFC0000, with a single entry interrupt vector at 0x80000000 and a stack that that grows downwards from 0xA001FFFC to 0xA0008000. 

Your kernel is to be located between 0x80000000 and 0x9FFFFFFF; with code starting from 0x9D000000 and the kernels' DMA-able data from 0xA0000000. 

Global variables and static local variables resides in the interval starting from 0xE0000000 up to but not including 0xFF200200.

## Compiling Using [LLVM](http://www.llvm.org) 5 and Later

To compile a program, normally you type something similar to 

    terminal$ clang-5.0 -g -mlong-calls -target mipsel -mips32r2 --std=c++17 -fblocks -c main.cpp -o main.o
    
    terminal$ ld.lld -T Loadscripts/PIC32MX795F512L.ld -o mykernel.elf main.o Specifics/PIC32MX795F512L.s start.o
    
    terminal$ llvm2pic32 mykernel.elf > mykernel.hex

Note that your kernel must contain a function having the signature  `int main() { ... }`.

To understand where in the virtual address space a symbol is defined, enter:

    terminal$ ninja syms | egrep 'T.*main' 
    9d000c40 T main

Other useful targets available in the build file are: 

    terminal$ ninja size
    text      data    bss   dec      hex filename
    57272     836     768   58876    e5fcmonitor_PIC32MX795F512L
    
    terminal$ ninja sect # Outputs all available sections
    terminal$ ninja list # The list file to stdout
    terminal$ ninja syms # All symbols on stdout 
    terminal$ ninja source # Ftns, virt. addr. and obj code.

## MIPS Assembly in Source Files

The following example illustrates how to include assembly code in your source files:

    #include <Freeform.h>
    
    int main()
    {
        static Basicblocks flash_1_1[] = {
            // TRISB @ 0xbf886040 ⬷ 0, i.e PORTB all output.
            MIPS_LUI (8, 0), 
            MIPS_ORI (8, 8, 0),
            MIPS_LUI (9, 0xbf88), 
            MIPS_SW (8u, 24640u, 9u), 
        // flash_again:
            MIPS_MFC0 (8u, 9u, 0u), // $8 ⬷ c0_count 
            MIPS_LUI (9, 0xbf88), 
            // LATB @ 0xbf886060 ⬷ count
            MIPS_SW (8u, 24672u, 9u), 
            MIPS_B (NEG16(4<<2))
        };

        typedef void (*Entry)();
        void (*entry)() = (Entry)flash_1_1;
        entry();
    }
 
## Traditional Platform Features 

`Monitor` contains non-threadsafe implementations of `malloc` at `0x9d009650` and `free` at `0x9d00521c` that can be called from your application with something similar to:

    MIPS_LUI (30, 40192)
    MIPS_ORI (30, 30, 21020)
    MIPS_J   (30)

There is also a `Disassemble` available at `0x9d001fe0`. See `Monitor.h` for details.

## System Calls, Breaks and Interrupts
 
The `syscall` handler has the following weak and therefore overridable prototype:

    extern "C" void Syscall(__builtin_uint_t arg);

The `break` handler is also overridable and defined as follows: 
    
     extern "C" void Break() { ; }

The interrupt handler function must have the signature  `extern "C" void Isr();` as in 

    extern "C" void Isr() {
        switch (INTSTAT_VEC_6͞ & 🔎(INTSTAT)) {
        case  0: break; // Core Timer Interrupt
        case  1: break; // Core Software Interrupt 0
        case 96: break; // Real Time Clock
        ... } 
        
        static Basicblocks bootisr_1_1[] = {
            MIPS_LUI (30, 40192)
            MIPS_ORI (30, 30, 21020)
            MIPS_J   (30)
        };
        typedef void (*BootISR)();
        void (*bootISR)() = (BootISR)bootisr_1_1;
        bootISR();
    }
        

See Table 7-1, *'INTERRUPT IRQ, VECTOR AND BIT LOCATION'* in [PIC32MX5XX/6XX/7XX Family Data Sheets](http://ww1.microchip.com/downloads/en/DeviceDoc/60001156J.pdf) for a list of available interrupts.


