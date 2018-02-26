# A MIPS/PIC32 Monitor Program

**This program helps you** upload your programs, interact with your chip, insert hardware breakpoints and see how control flows when your program is run.

[**monitor_PIC32MX470F512L.hex**][af333][^1] Enables the chips' UART5 where port RF13 (pin 39) is U5TX, and port RF12 (pin 40) is U5RX. This binary can upload programs up to 64 kB of size. 

[**nomonitor_MX.hex**][c381b] The same startup code as the application above, but without its debugging capabilities.

[af333]: https://github.com/andeha/Monitor/Releases/monitor_PIC32MX470F512L_af333.hex
[c381b]: https://github.com/andeha/Monitor/Releases/nomonitor_MX_c381b.hex

## How to Connect a PIC32 to a PC or a Mac 

To interact with your USB-port equipped PIC32-board via a USB cable connected to your computer, do the following:
	
On a **Mac**, start a VT100 terminal emulator with something similar to:

    prompt% screen /dev/cu.usbserial-A506K4XV 600

On a **PC**, use hyperterm or putty. 

## About the Startup

The startup code in **monitor** and **nomonitor** resides starting at 0xBFC0000, with a single entry interrupt vector at 0x80000000 and a stack that that grows downwards from 0xA001FFFC to 0xA0008000. 

Your kernel is to be located between 0x80000000 and 0x9FFFFFFF; with code starting from 0x9D000000 and the kernels' DMA-able data from 0xA0000000. 

Global variables and static local variables resides in the interval starting from 0xE0000000 up to but not including 0xFF200200.

## Interrupts
 
The function processing interrupts must have the prototype `extern "C" void Isr();` as in 

    extern "C" void Isr() {
        switch (INTSTAT_VEC_6͞ & 🔎(INTSTAT)) {
        case  0: break; // Core Timer Interrupt
        case  1: break; // Core Software Interrupt 0
        case 96: break; // Real Time Clock
    ...
    