//
//  Monitor.h
//  The MIPS/PIC32 Monitor
//

#ifndef __MONITOR_H
#define __MONITOR_H

#pragma mark Snapshot from Pinecone

#define NULL 0

typedef unsigned char uint8_t;

#ifdef  __mips__
typedef unsigned long long  uint32_t;
typedef long long           int32_t;
typedef uint32_t            __builtin_uint_t;
typedef int32_t             __builtin_int_t;
#elif __x86_64__
typedef unsigned long long  uint64_t;
typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef uint64_t            __builtin_uint_t;
typedef int64_t             __builtin_int_t;
#endif

typedef unsigned short      uint16_t;

template <typename T>
struct Optional {

    union Option { T inner; __builtin_uint_t no; Option() {};  ~Option() {} } option;

    Optional(T inner) { option.inner = inner; kind = 0; }

    Optional(const Optional& other) { kind = other.kind; }

    Optional() { kind = 1; }

    static Optional no() { return Optional(); }

    T * query() const { if (kind != 1) return (T*)&option.inner; else return 0; }

    __builtin_uint_t kind;

};

typedef union { // 2^–14 to 2^15 or 3.1 × 10^–5 to 6.50 × 10^4
    struct { uint8_t lsb; uint8_t msb; } little_endian;
    struct { uint8_t msb; uint8_t lsb; } big_endian;
    struct {
        unsigned mantissa : 10;
        unsigned exponent :  5;
        unsigned sign     :  1;
    } ieee754;
    uint16_t shortword;
} half;

typedef union {
    float value; // 2^–126 to 2^127 or 1.18 × 10^–38 to 3.40 × 10^38
    struct { half lsh; half msh; } little_endian;
    struct { half msh; half lsh; } big_endian;
    struct {
        unsigned mantissa : 24;
        unsigned exponent :  8;
        unsigned sign     :  1;
    } ieee754;
    uint32_t longword;
} longword;

typedef union {
    double value; // 2^–1022  2^1023 or 2.23 × 10^–308 to 1.79 × 10^308
    struct { uint32_t lsl; int32_t msl; } little_endian;
    struct { int32_t msl; uint32_t lsl; } big_endian;
    struct { uint32_t lsl; uint32_t msl; } unsigned_little_endian;
    struct { uint32_t msl; uint32_t lsl; } unsigned_big_endian;
    struct {
        unsigned mantissal : 32;
        unsigned mantissah : 20;
        unsigned exponent  : 11;
        unsigned sign      :  1;
    } ieee754;
#ifdef __x86_64__
    uint64_t quad;
#endif
} quad;

#ifdef  __mips__
typedef quad uint64_t;
typedef quad int64_t;
#endif

#define BITMASK(type) enum : type
#define INLINED __attribute__((always_inline))
#define MACRO inline INLINED

MACRO __builtin_uint_t 🔎(__builtin_uint_t var) {
    return *((__builtin_uint_t *)var); }

MACRO __builtin_uint_t&  🔧(__builtin_uint_t var) {
    return (__builtin_uint_t&)*(__builtin_uint_t *)var; }

MACRO __builtin_uint_t TrailingZeros(__builtin_uint_t x) { if (x == 0) { return
    8*sizeof(x); } __builtin_uint_t zeros = 0, mask = 1; while (!(x&mask)) {
    zeros++; mask<<=1; } return zeros; }

MACRO __builtin_uint_t MaskAndShift(__builtin_uint_t value, __builtin_uint_t
    mask) { __builtin_uint_t shift = TrailingZeros(mask); return (mask&value)
    >> shift; }

MACRO __builtin_uint_t 🔎MaskandShift(__builtin_uint_t var, __builtin_uint_t
    mask) { return MaskAndShift(🔎(var), mask); }

MACRO void 🔧(__builtin_uint_t var, __builtin_uint_t mask, __builtin_uint_t 
	value) { __builtin_uint_t shift = TrailingZeros(mask);
    __builtin_uint_t secured = value & (mask>>shift);
    *(__builtin_uint_t *)var &= ~mask;
    *(__builtin_uint_t *)var |= secured<<shift; }

MACRO void 🔧Toggle(__builtin_uint_t var, __builtin_uint_t msk, 
	__builtin_uint_t val) { __builtin_uint_t shift = TrailingZeros(msk);
    __builtin_uint_t secured = val & (msk>>shift);
    *(__builtin_uint_t *)var ^= secured<<shift; }

extern "C" int printf(const char *eightBitFormat, ...);

#pragma mark - MIPS/PIC32

#pragma mark Resets (0xBF80F600)

#define RCON    0xBF80F600 // Reset Control Register
#define RSWRST  0xBF80F610 // Software Reset Register

BITMASK (uint32_t) { // RCON
    RCON_CMR    = 0b1000000000, // Configuration Mismatch Reset
    RCON_VREGS  = 0b0100000000, // Voltage Regulator Standby Enable
    RCON_EXTR   = 0b0010000000, // External Reset (MCLR) Pin Flag
    RCON_SWR    = 0b0001000000, // Software Reset Flag
    RCON_WDTO   = 0b0000010000, // Watchdog Timer Time-out Flag
    RCON_SLEEP  = 0b0000001000, // Wake From Sleep Flag
    RCON_IDLE   = 0b0000000100, // Wake From Idle Flag
    RCON_BOR    = 0b0000000010, // Brown-out Reset Flag
    RCON_POR    = 0b0000000001  // Power-on Reset Flag
};

BITMASK (uint32_t) { // RSWRST
    RSWRST_SWRST = 0b0000000001 // Software Reset Trigger
};

#pragma mark Interrupts (0xBF881000)

#define INTSTAT     0xBF881010 // Interrupt Status Register
#define INTSTATCLR  0xBF881014
#define INTSTATSET  0xBF881018

BITMASK (uint32_t) { // INTSTAT
    INTSTAT_SRIPL_3͞ = 0b11100000000, // Requested Priority Level
    INTSTAT_VEC_6͞   = 0b00000111111  // Interrupt Vector
};

#pragma mark The UART 1 - UART 6 (0xBF806000)

#define U5MODE          0xBF806a00 // UARTx Mode Register
#define U5STA           0xBF806a10 // UARTx Status and Control Register
#define U5TXREG         0xBF806a20 // UARTx Transmit Register
#define U5RXREG         0xBF806a30 // UARTx Receive Register
#define U5BRG           0xBF806a40 // UARTx Baud Rate Register

BITMASK (uint32_t) { // UxMODE
    UxMODE_ON       = 0b1000000000000000, // UARTx Enable bit
    UxMODE_SIDL     = 0b0010000000000000, // Stop in Idle Mode
    UxMODE_IREN     = 0b0001000000000000, // IrDA Encoder and Decoder Enable
    UxMODE_RTSMD    = 0b0000100000000000, // Mode Selection for UxRTS Pin bit
    UxMODE_UEN_2͞    = 0b0000011000000000, // UARTx Enable
    UxMODE_WAKE     = 0b0000000010000000, // Enable Wake-up on Start bit Detect During Sleep Mode
    UxMODE_LPBACK   = 0b0000000001000000, // UARTx Loopback Mode Select
    UxMODE_ABAUD    = 0b0000000000100000, // Auto-Baud Enable
    UxMODE_RXINV    = 0b0000000000010000, // Receive Polarity Inversion
    UxMODE_BRGH     = 0b0000000000001000, // High Baud Rate Enable
    UxMODE_PDSEL_2͞  = 0b0000000000000110, // Parity and Data Selection. E.g 00 = 8-bit data, no parity
    UxMODE_STSEL    = 0b0000000000000001  // Stop Selection. 1 = 2 Stop bits. 0 = 1 Stopbit
};

BITMASK (uint32_t) { // UxSTA
    UxSTA_ADM_EN    = 0b1000000000000000000000000, // Automatic Address Detect Mode Enable
    UxSTA_ADDR_8͞    = 0b0111111110000000000000000, // Automatic Address Mask
    UxSTA_UTXISEL_2͞	= 0b0000000001100000000000000, // TX Interrupt Mode Selection
    UxSTA_UTXINV    = 0b0000000000010000000000000, // Transmit Polarity Inversion
    UxSTA_URXEN     = 0b0000000000001000000000000, // Receiver Enable
    UxSTA_UTXBRK    = 0b0000000000000100000000000, // Transmit Break
    UxSTA_UTXEN     = 0b0000000000000010000000000, // Transmit Enable
    UxSTA_UTXBF     = 0b0000000000000001000000000, // Transmit Buffer Full Status
    UxSTA_TRMT      = 0b0000000000000000100000000, // Transmit Shift Register is Empty
    UxSTA_URXISEL_2͞ = 0b0000000000000000011000000, // Receive Interrupt Mode Selection
    UxSTA_ADDEN     = 0b0000000000000000000100000, // Address Character Detect
    UxSTA_RIDLE     = 0b0000000000000000000010000, // Receiver Idle
    UxSTA_PERR      = 0b0000000000000000000001000, // Parity Error Status
    UxSTA_FERR      = 0b0000000000000000000000100, // Framing Error Status
    UxSTA_OERR      = 0b0000000000000000000000010, // Receive Buffer Overrun Error Status
    UxSTA_URXDA     = 0b0000000000000000000000001  // Receive Buffer Data Available
};

BITMASK (uint32_t) { // UxTXREG
    UxRXREG_TX8   = 0b100000000,
    UxTXREG_TX_8͞  = 0b011111111 // Transmit register
};

BITMASK (uint32_t) { // UxRXREG
    UxRXREG_RX8   = 0b100000000,
    UxTXREG_RX_8͞  = 0b011111111 // Recieve register
};

BITMASK (uint32_t) { // UxBRG
    UxBRG_BRG_1͞6͞ = 0b1111111111111111 // Baud Rate Divison
};

#pragma mark General purpose I/O PORT A - PORT G (0xBF886000, 0xBF886040, ...)

/*  TRIS is a Data Direction or Tri-State Control register that determines 
 whether a digital pin is an input or an output. '1' configures the 
 corresponding I/O pin as an input. */ 
#define TRISA       0xBF886000
#define TRISACLR    0xBF886004
#define TRISASET    0xBF886008
#define TRISAINV    0xBF88600C
/*  PORT is the register used to read the current state of the signal applied to	
 the port I/O pins. */
#define PORTA       0xBF886010
#define PORTACLR    0xBF886014
#define PORTASET    0xBF886018
#define PORTAINV    0xBF88601C
/*  LAT is the register used to write data to the port I/O pins. Reading the LATx
 Latch register reads the last value written to the corresponding PORTor Latch 
 register. */ 
#define LATA        0xBF886020
#define LATACLR     0xBF886024
#define LATASET     0xBF886028
#define LATAINV     0xBF88602C
#define ODCA        0xBF886030
#define ODCACLR     0xBF886034
#define ODCASET     0xBF886038
#define ODCAINV     0xBF88603C

/**

'Every I/O module register has a corresponding Clear (CLR), Set (SET) and
 Invert (INV) register providing atomic bit manipulation. A value written to
 a SET, CLR or INV register effectively performs the implied operation,
 but only on the corresponding base register and only bits specified as
 ‘1’ are modified. Bits specified as ‘0’ are not modified.

 Pins are configured as digital inputs by setting the corresponding TRIS
 register bits = 1. When configured as inputs, they are either TTL buffers or
 Schmitt Triggers. Several digital pins share functionality with analog
 inputs and default to the analog inputs at POR. Setting the corresponding
 bit in the AD1PCFG register = 1 enables the pin as a digital pin.

 Certain pins can be configured as analog inputs/outputs used by the ADC and
 comparator modules. Setting the corresponding bits in the AD1PCFG register
 equal to '0' enables the pin as an analog input pin and must have the
 corresponding TRIS bit set = 1 (input).

 Pins are configured as digital outputs by setting the corresponding TRIS
 register bits = 0. When configured as digital outputs, these pins are CMOS
 drivers or can be configured as open-drain outputs by setting the
 corresponding bits in the Open-Drain Configuration (ODCx) register.'

*/

#define PortSetDirection(X, tris)           (*((uint32_t *)TRIS##X) = (uint16_t)(tris))
#define PortGetDirection(X)                 (TRIS##X)
#define PortRead(X)                         (PORT##X)
#define PortReadAndMask(port,mask)          PortRead(port) & mask
#define PortWrite(X, lat)                   (*((uint32_t *)LAT##X) = (uint32_t)(lat))
#define PortSetBits(X, bits)                (*((uint32_t *)LAT##X##SET) = (uint32_t)(bits))
#define PortClearBits(X, bits)              (*((uint32_t *)LAT##X##CLR) = (uint32_t)(bits))
#define PortToggleBits(X, bits)             (*((uint32_t *)LAT##X##INV) = (uint32_t)(bits))
#define PortOpenDrainOpen(X, bits)          (ODC##X##SET = (unsigned int)(bits), TRIS##X##CLR = (unsigned int)(bits))
#define portOpenDrainClose(X, bits)         (ODC##X##CLR = (unsigned int)(bits), TRIS##X##SET = (unsigned int)(bits))
#define PortCloseAll(X)                     (TRIS##X##SET = 0xFFFFFFFF, LAT##X##CLR = 0xFFFFFFFF)
#define PortCloseBits(X, bits)              (TRIS##X##SET = (unsigned int)(bits), LAT##X##CLR = (unsigned int)(bits))
#define PortSetPinsAnalogOut(X, outputs)    (TRIS##X##CLR = (unsigned int)(outputs), ANSEL##X##SET = (unsigned int)(outputs))
#define PortSetPinsAnalogIn(X, inputs)      (TRIS##X##SET = (unsigned int)(inputs),  ANSEL##X##SET = (unsigned int)(inputs))
#define PortSetPinsDigitalOut(X, outputs)   (TRIS##X##CLR = (unsigned int)(outputs), ANSEL##X##CLR = (unsigned int)(outputs))
#define PortSetPinsDigitalIn(X, inputs)     (TRIS##X##SET = (unsigned int)(inputs),  ANSEL##X##CLR = (unsigned int)(inputs))
#define PortRectifyAsInputs(X, tris)        (*((uint32_t *)TRIS##X##SET) = (uint16_t)(tris))

#pragma mark Config

MACRO __builtin_uint_t 🔎Config3() { __builtin_uint_t val; asm
    volatile("mfc0 %0, $16, 3; nop" : "=r" (val)); return val; }

BITMASK (uint32_t) { // Config3
    Config3_ITL  = 0b100000000, // Indicates that IFlowTrace hardware is present
    Config3_VEIC = 0b001000000, // Support for an external interrupt controller is implemented
    Config3_VInt = 0b000100000, // Vectored interrupts implemented
    Config3_SP   = 0b000010000, // Small (1kB) page support is implemented, and the PageGrain register exists
    Config3_SM   = 0b000000010, // SmartMIPSTM ASE implemented
    Config3_TL   = 0b000000001  // Trace Logic implemented
};

#pragma mark EJTAG (When the MIPS is in Debug Mode (drseg or 0xFF300000 if in debug mode))

MACRO __builtin_uint_t 🔎Debug() { __builtin_uint_t val; asm
    volatile("mfc0 %0, $23, 0; nop" : "=r" (val)); return val; }

/* CP0, register 23, 0 */ BITMASK(uint32_t) { // DEBUG: Debug control, exception status and EJTAG trace control
    DEBUG_DBD        = 0b10000000000000000000000000000000, // Branch ͞ Debug Exception
    DEBUG_DM         = 0b01000000000000000000000000000000, // Debug Mode
    DEBUG_NODCR      = 0b00100000000000000000000000000000, // Debug Control Register
    DEBUG_LSNM       = 0b00010000000000000000000000000000, // Load Store Access Control
    DEBUG_DOZE       = 0b00001000000000000000000000000000, // Low-Power Mode Debug Exception
    DEBUG_HALT       = 0b00000100000000000000000000000000, // System Bus Clock Stop
    DEBUG_COUNTDM    = 0b00000010000000000000000000000000, // Count Register Behavior
    DEBUG_IBUSEP     = 0b00000001000000000000000000000000, // Instruction Fetch Bus Error Exception Pending
    DEBUG_MCHECKP    = 0b00000000100000000000000000000000, // Machine Check Exception Pending
    DEBUG_CACHEEP    = 0b00000000010000000000000000000000, // Cache Error Pending
    DEBUG_DBUSEP     = 0b00000000001000000000000000000000, // Data Access Bus Error Exception Pending
    DEBUG_IEXI       = 0b00000000000100000000000000000000, // Imprecise Error Exception Inhibit Control
    DEBUG_DDBSIMPR   = 0b00000000000010000000000000000000, // Debug Data Break Store Exception
    DEBUG_DDBLIMPR   = 0b00000000000001000000000000000000, // Debug Data Break Load Exception
    DEBUG_VER_3͞      = 0b00000000000000111000000000000000, // EJTAG Version
    DEBUG_DEXCCODE_5͞ = 0b00000000000000000111110000000000, // Latest Exception in Debug Mode
    DEBUG_NOSST      = 0b00000000000000000000001000000000, // Singe-step Feature Control
    DEBUG_SST        = 0b00000000000000000000000100000000, // Debug Single-step Control
    DEBUG_DIBIMPR    = 0b00000000000000000000000001000000, // Imprecise Debug Instruction Break Exception
    DEBUG_DINT       = 0b00000000000000000000000000100000, // Debug Interrupt Exception
    DEBUG_DIB        = 0b00000000000000000000000000010000, // Debug Instruction Break Exception
    DEBUG_DDBS       = 0b00000000000000000000000000001000, // Debug Data Break Exception on Store
    DEBUG_DDBL       = 0b00000000000000000000000000000100, // Debug Data Break Exception on Load
    DEBUG_DBP        = 0b00000000000000000000000000000010, // Debug Software Breakpoint Exception
    DEBUG_DSS        = 0b00000000000000000000000000000001  // Debug Single-step Exception
};

MACRO void 🔧Debug( __builtin_uint_t mask, __builtin_uint_t value) {
    __builtin_uint_t var; 🔧((__builtin_uint_t)&var, mask, value);
    asm volatile("mtc0 %0, $23, 0; nop" : : "r" (var)); }

MACRO bool HasDCR() { return !(DEBUG_NODCR & 🔎Debug()); }
MACRO bool HasSingleStep() { return !(DEBUG_NOSST & 🔎Debug()); }

#define DCR 0xFFFFFFFFFF200000 // Debug Control Register

BITMASK(uint32_t) { // DCR
    DCR_ENM     = 0b1000000000000000000000000000000, // Endianess in Kernel and Debug mode
    DCR_DB      = 0b0000000000000100000000000000000, // Data Break Implemented
    DCR_IB      = 0b0000000000000010000000000000000, // Instruction Break Implemented
    DCR_IVM     = 0b0000000000000001000000000000000, // Inverted Value Match
    DCR_DVM     = 0b0000000000000000100000000000000, // Data Value Match Register
    DCR_CBRK    = 0b0000000000000000000010000000000, // Indicates that Complex Breakpoint logic is implemented
    DCR_PCS     = 0b0000000000000000000001000000000, // Program Counter Sampling implemented
    DCR_PCR_3͞   = 0b0000000000000000000000111000000, // PC Sampling Rate
    DCR_PCE     = 0b0000000000000000000000000100000, // PC Sampling Enable
    DCR_INTE    = 0b0000000000000000000000000010000, // Interrupt Enable in Normal Mode
    DCR_NMIE    = 0b0000000000000000000000000001000, // Non-Maskable Interrupt Enable for non-debug mode
    DCR_NMIP    = 0b0000000000000000000000000000100, // NMI Pending Indication
    DCR_SRE     = 0b0000000000000000000000000000010, // Soft Reset Enable
    DCR_PE      = 0b0000000000000000000000000000001  // Probe Enable, indicates if EJTAG_debug is expected to be found at 0xBFC0 0480 or 0xFF20 0200
};

// Instruction Breakpoints 0-15

#define IBS 0xFFFFFFFFFF301000 // Instruction Breakpoint Status

BITMASK(uint32_t) { // IBS
    IBS_ASIDSUP = 0b1000000000000000000000000000000, // Indicates that ASID compare is supported in instruction breakpoints
    IBS_BCN_4͞   = 0b0011110000000000000000000000000, // Number of instruction breakpoints implemented
    IBS_BS_6͞    = 0b0000000000000000000000000111111  // Break status for breakpoint n is at BS[n], with n from 0 to 5b
};

MACRO void 🔧IBA(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF301100 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint Address n

MACRO void 🔧IBM(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF301108 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint Address Mask n

MACRO void 🔧IBASID(unsigned int oneToFifteen, __builtin_uint_t mask,
     __builtin_uint_t value) { 🔧(0xFF301110 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint ASID n

MACRO void 🔧IBC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF301118 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint Control n

MACRO void 🔧IBCC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF301120 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint Complex Control n

MACRO void 🔧IBPC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF301128 + 0x100*(oneToFifteen - 1), mask, value); } // Instruction Breakpoint Pass Counter n

BITMASK(uint32_t) { // IBCx
    IBCx_ASIDuse = 0b100000000000000000000000, // Use ASID value in compare for instruction breakpoint n
    IBCx_TE      = 0b000000000000000000000100, // Use instruction breakpoint n as trigger point
    IBCx_BE      = 0b000000000000000000000001  // Use instruction breakpoint
};

BITMASK(uint32_t) { // IBCCx
    IBCCx_PrCnd_4͞   = 0b000000000011110000000000, // Priming condition for I Breakpoint n
    IBCCx_CBE       = 0b000000000000001000000000, // Complex break enable
    IBCCx_DBrkNum_4͞ = 0b000000000000000111100000, // Indicates which data breakpoint channel is used
    IBCCx_Q         = 0b000000000000000000010000  // Qualify this breakpoint based on the data breakpoint indicated in DBrkNum
};

BITMASK(uint32_t) { // IBPCx
    IBPCx_PASSCOUNT_8͞ = 0b11111111 // Prevents a break/trigger action until the matching conditions on breakpoint n have been seen this number of times
};

MACRO __builtin_uint_t InstBrk() { return DCR_IB & 🔎(DCR) ? 🔎MaskandShift(IBS, IBS_BCN_4͞) : 0; }

// Data Breakpoints 0-15

#define DBS 0xFFFFFFFFFF302000 // Data Breakpoint Status

BITMASK(uint32_t) { // DBS
    DBS_ASID   = 0b01000000000000000000000000000000, // Indicates that ASID compares are supported in data breakpoints.
    DBS_BCN_4͞  = 0b00000000000000001111000000000000, // Number of data breakpoints implemented
    DBS_BS     = 0b00000000000000000000000000000001  // Break status for breakpoint n is at BS[n]
};

MACRO void 🔧DBA(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302100 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Address n
MACRO void 🔧DBM(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302108 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Address Mask n
MACRO void 🔧DBASID(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302110 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint ASID n
MACRO void 🔧DBC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302118 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Control n
MACRO void 🔧DBV(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302120 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Value n
MACRO void 🔧DBCC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302128 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Complex Control n
MACRO void 🔧DBPC(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302130 + 0x100*(oneToFifteen - 1), mask, value); } // Data Breakpoint Pass Counter n
MACRO void 🔧DVM(unsigned int oneToFifteen, __builtin_uint_t mask,
    __builtin_uint_t value) { 🔧(0xFF302200 + 0x100*(oneToFifteen - 1), mask, value); } // Data value match register n

BITMASK(uint32_t) { // DBASID
    DBASID_ASID_8͞ = 0b11111111 // Data breakpoint ASID value for compares
};

BITMASK(uint32_t) { // DBCn
    DBCx_ASIDuse = 0b1000000000000000000000000000, // Use ASID value in compare for data breakpoint n
    DBCx_BAI_4͞   = 0b0000001111000000000000000000, // Byte access ignore controls ignore of access to a specific byte
    DBCx_NoSB    = 0b0000000000000010000000000000, // Controls if condition for data breakpoint is not fulfilled on a store transaction
    DBCx_NoLB    = 0b0000000000000001000000000000, // Controls if condition for data breakpoint is not fulfilled on a load transaction
    DBCx_BLM_4͞   = 0b0000000000000000000011110000, // Byte lane mask for value compare on data breakpoint
    DBCx_TE      = 0b0000000000000000000000000100, // Use data breakpoint n as triggerpoint
    DBCx_IVM     = 0b0000000000000000000000000010, // Invert Value Match
    DBCx_BE      = 0b0000000000000000000000000001  // Use data breakpoint as breakpoint
};

BITMASK(uint32_t) { // DBCCx
    DBCCx_TIBrkNum = 0b000000000000000000000000, // Tuple Instruction Break Number
    DBCCx_TUP      = 0b000000000000000000000000, // Tuple Enable
    DBCCx_PrCnd    = 0b000000000000000000000000, // Priming condition for D Breakpoint n
    DBCCx_CBE      = 0b000000000000000000000000, // Complex Break Enable
    DBCCx_DQBrkNum = 0b000000000000000000000000, // Indicates which data breakpoint channel is used to qualify this data breakpoint
    DBCCx_DQ       = 0b000000000000000000000000  // Qualify this breakpoint based on the data breakpoint indicated in DBrkNum
};

BITMASK(uint32_t) { // DBPCx
    DBPCx_PASSCOUNT_1͞6͞ = 0b1111111111111111 // Prevents a break/trigger action until the matching conditions on breakpoint n have been seen this number of times
};

MACRO __builtin_uint_t DataBrk() { return DCR_DB & 🔎(DCR) ? 🔎MaskandShift(DBS, DBS_BCN_4͞) : 0; }

#pragma mark Complex Breakpoint

#define CBTC 0xFFFFFFFFFF308000 // Complex Break and Triggerpoint Control

BITMASK(uint32_t) { // CBTC
    CBTC_STMode = 0b100000000, // Stopwatch Timer Mode
    CBTC_STP    = 0b000010000, // Stopwatch Timer Present
    CBTC_PP     = 0b000001000, // Priming Present
    CBTC_DQP    = 0b000000100, // Data Qualifiy Present
    CBTC_TP     = 0b000000010, // Tuple Present
    CBTC_PCP    = 0b000000001  // Pass Counters Present
};

MACRO void 🔧PrCndAI(unsigned int zeroToFive, uint32_t mask,
    uint32_t value) { 🔧(0xFF308300 + 0x20*zeroToFive, mask, value); }

MACRO void 🔧PrCndAD(unsigned int zeroToFive, uint32_t mask,
    uint32_t value) { 🔧(0xFF3084e0 + 0x20*zeroToFive, mask, value); }

BITMASK(uint32_t) { // PrCndAIxorDx
    PrCndAIx_Cond3 = 0xFF000000,
    PrCndAIx_Cond2 = 0x00FF0000,
    PrCndAIx_Cond1 = 0x0000FF00,
    PrCndAIx_Cond0 = 0x000000FF
};

#pragma mark Stopwatch Timer Control

#define STCtl 0xFFFFFFFFFF308900 // Stopwatch Timer Control
#define STCnt 0xFFFFFFFFFF308908 // Stopwatch Timer Count

BITMASK(uint32_t) { // STCtl
    STCtl_StopChan1_4͞  = 0b000000111100000000000000,
    STCtl_StartChan1_4͞ = 0b000000000011110000000000,
    STCtl_En1          = 0b000000000000001000000000,
    STCtl_StopChan0_4͞  = 0b000000000000000111100000,
    STCtl_StartChan0_4͞ = 0b000000000000000000011110,
    STCtl_En0          = 0b000000000000000000000001
};

MACRO bool HasComplexBrk() { return DCR_CBRK & 🔎(DCR) ? 1 : 0; }
MACRO bool HasStopwatch() { return CBTC_STP & 🔎(CBTC) ? 1 : 0; }
MACRO bool HasPrimingBreakpoint() { return CBTC_PP & 🔎(CBTC) ? 1 : 0; }
MACRO bool HasDataQualifiy() { return CBTC_DQP & 🔎(CBTC) ? 1 : 0; }
MACRO bool HasTuple() { return CBTC_TP & 🔎(CBTC) ? 1 : 0; }
MACRO bool HasPassCounters() { return CBTC_PCP & 🔎(CBTC) ? 1 : 0; }

#pragma mark iFlowtrace

MACRO bool HasiFlowTrace() { return Config3_ITL & 🔎Config3(); }

#define ITCBTW_LO 0xFF303F80 // Used to read Trace Words from the on-chip trace memory
#define ITCBTW_HI 0xFF303F84 // Used to read Trace Words from the on-chip trace memory
#define ITCBRDP   0xFF303F88 // The address pointer to on-chip trace memory
#define ITCBWRP   0xFF303F90 // Points to the location where the next new TW for on-chip trace will be written. The top bit in the register indicates whether the pointer has wrapped

BITMASK(uint32_t) { // ITCBWRP
    ITCBWRP_Wrap    = 0b10000000000000000000000000000000, // Trace wrapped
    ITCBWRP_ADDR_3͞1͞ = 0b01111111111111111111111111111111  // Byte address of the next on-chip trace memory word to be written
};

#define IFCTL 0xFFFFFFFFFF303FC0 // iFlowTrace Control Block Control/Status

BITMASK(uint32_t) { // IFCTL
    IFCTL_CYC      = 0b100000000000000, // Delta Cycle Mode
    IFCTL_FDT      = 0b010000000000000, // Filtered Data Trace mode
    IFCTL_BM       = 0b001000000000000, // Breakpoint Match
    IFCTL_ER       = 0b000100000000000, // Trace exception returns
    IFCTL_FCR      = 0b000010000000000, // Trace function calls and returns
    IFCTL_EST      = 0b000001000000000, // Enable special tracing mode
    IFCTL_SYP      = 0b000000111100000, // Synchronization Period
    IFCTL_OFCLK    = 0b000000000010000, // Controls the off-chip clock ratio
    IFCTL_OFC      = 0b000000000001000, // Off chip
    IFCTL_IO       = 0b000000000000100, // Inhibit overflow
    IFCTL_EN       = 0b000000000000010, // Trace enable
    IFCTL_ON       = 0b000000000000001  // Software control of trace collection
};

#define ITCBADDR 0xFFFFFFFFFF303FC8 // iFlowTrace Control Block Trace write address pointer

BITMASK(uint32_t) { // ITCBADDR
    ITCBADDR_Wrap        = 0b10000000000000000000000000000000, // Trace wrapped
    ITCBADDR_Waddr_3͞1͞    = 0b01111111111111111111111111111111  // When trace concludes, WAddr contains the first address in trace memory not yet written.
};

#define ITrigiFlowTrcEn 0xFFFFFFFFFF303FD0 // Instruction-break Trigger iFlowtrace Enable register
#define DTrigiFlowTrcEn 0xFFFFFFFFFF303FD8 // Data-break Trigger iFlowtrace Enable register

BITMASK(uint32_t) { // I/DTrigiFlowTrcEn
    TrigiFlowTrcEn_EN    = 0b10000000000000000000000000000, // Whether the trigger signal from EJTAG simple or complex instruction (data or tuple) break should trigger iFlowTrace tracing functions or not.
    TrigiFlowTrcEn_IDBrk = 0b00000000000000111111111111111  // Specify which instruction (data or tuple) breaks enable or disable iFlowTrace. A value of 0 implies that trace is turned off (unconditional trace stop) and a value of 1 specifies that the trigger enables trace (unconditional trace start).
};

MACRO void 🔧UserTraceData1(uint32_t value) { asm
    volatile("mtc0 %0, $23, 3; nop" : : "r" (value)); }

MACRO void 🔧UserTraceData2(uint32_t value) { asm
    volatile("mtc0 %0, $24, 3; nop" : : "r" (value)); }

#pragma mark - MIPS32 Debug State

MACRO uint32_t 🔎Debug2() { uint32_t val; asm
    volatile("mfc0 %0, $23, 5; nop" : "=r" (val)); return val; }

/* CP0, register 23, 5 */ BITMASK(uint32_t) { // DEBUG2:
    // Additional information about Complex Breakpoint exceptions
    DEBUG2_PRM  = 0b00000000000000000000000000001000, // Primed
    DEBUG2_DQ   = 0b00000000000000000000000000000100, // Data Qualified
    DEBUG2_TUP  = 0b00000000000000000000000000000010, // Tuple Breakpoint
    DEBUG2_PACO = 0b00000000000000000000000000000001  // Pass Counter
};

MACRO uint32_t 🔎Depc() { uint32_t val; asm
    volatile("mfc0 %0, $24, 0; nop" : "=r" (val)); return val; }

MACRO uint32_t 🔎Desave() { uint32_t val; asm
    volatile("mfc0 %0, $31, 0; nop" : "=r" (val)); return val; }

#pragma mark - The MIPS/PIC32 Monitor Debugging API

enum class TraceMethod {
    Normal,                                 // Each assembly instruction is traced
    DeltaCycle,                             // Each trace message will include a 10b delta cycle value which reports the number of cycles that have elapsed since the last message was generated. A value of 0 indicates that the two messages were generated in the same cycle. A value of 1 indicates that they were generated in consecutive cycles. If 1023 cycles elapse without an event being traced, a counter rollover message is generated.
    BreakpointMatch,                        // Instead of starting or stopping trace, a triggerpoint will cause a single breakpoint match trace record. This record indicates that there was a triggerpoint match, the breakpoint ID of the matching breakpoint, and the PC value of an instruction that matched the instruction of data breakpoint.
    FilteredData,                           // Rather than starting or stopping trace as in normal trace mode, a data triggerpoint will cause a filtered data trace record. This record indicates that there was a data triggerpoint match, the breakpoint ID of the matching breakpoint, whether it was a load or store, the size of the request, low order address bits, and the data value.
    CallsAndExceptions, Calls, Exceptions   // In this mode, the PC value of function calls and returns and/or exceptions and returns are traced out.
};

MACRO void StartTrace(TraceMethod method) { // Requires processor to be in debug mode
    if (!HasiFlowTrace()) { return; }
    🔧(IFCTL, IFCTL_ON, 0b1);
    switch (method) {
        case TraceMethod::Normal:
            🔧(IFCTL, IFCTL_EST, 0b0);
            break;
        case TraceMethod::DeltaCycle:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_CYC, 0b1);
            break;
        case TraceMethod::BreakpointMatch:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_BM, 0b1);
            break;
        case TraceMethod::FilteredData:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_FDT, 0b1);
            break;
        case TraceMethod::CallsAndExceptions:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_FCR, 0b1);
            🔧(IFCTL, IFCTL_ER, 0b1);
            break;
        case TraceMethod::Calls:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_FCR, 0b1);
            break;
        case TraceMethod::Exceptions:
            🔧(IFCTL, IFCTL_EST, 0b1);
            🔧(IFCTL, IFCTL_ER, 0b1);
            break;
    }
    🔧(ITCBADDR) = 0xA0000000 + 0x1000; // The location where the trace log is kept.
    🔧(IFCTL, IFCTL_EN, 0b1);
}

MACRO
void
SimpleInstructionBreakpoint(
    unsigned int oneToFifteen,
    __builtin_uint_t * addr,
    bool useAsTriggerPoint // Use instruction breakpoint n as trigger point
)
{
    if (oneToFifteen >= InstBrk()) { return; }
    🔧IBA(oneToFifteen, 0xFFFFFFFF, (__builtin_uint_t)addr);
    🔧IBM(oneToFifteen, 0xFFFFFFFF, 0xFFFFFFFF);
    🔧IBC(oneToFifteen, IBCx_TE, useAsTriggerPoint ? 0b1 : 0b0);
    🔧IBC(oneToFifteen, IBCx_BE, 0b1);
    // A Debug Instruction Break exception occurs later in EJTAG_debug. Use
    // 🔎MaskandShift(IBS, IBS_BS_6͞) to retrive the breakpoint number.
    // 0xFFFF FFFF BFC0 0480 or 0xFFFF FFFF FF20 0200 in dmseg depeding on if
    // ProbTrap bit in the EJTAG Control Register (ECR).
}

MACRO
void
ComplexInstructionBreakpoint(
    unsigned int oneToFifteen,
    __builtin_uint_t * addr,
    bool useAsTriggerPoint,
    unsigned int zeroOrPreceedingOneToFifteen, // Priming conditions provide a way to have one breakpoint enabled by another one
    unsigned int zeroOrDataBreakpointChannel, // In qualified mode, a breakpoint will recognize its conditions only after the specified data breakpoint matches both address and data
    uint32_t passcount
)
{
    if (oneToFifteen >= InstBrk()) { return; }
    SimpleInstructionBreakpoint(oneToFifteen, addr, useAsTriggerPoint);
    🔧IBCC(oneToFifteen, IBCCx_PrCnd_4͞, zeroOrPreceedingOneToFifteen);
    if (zeroOrDataBreakpointChannel) {
        🔧IBCC(oneToFifteen, IBCCx_DBrkNum_4͞, zeroOrDataBreakpointChannel); // Indicates which data breakpoint channel is used
    }
    🔧IBCC(oneToFifteen, IBCCx_Q, zeroOrDataBreakpointChannel ? 0b1 : 0b0); // Qualify this breakpoint based on the data breakpoint indicated in DBrkNum
    🔧IBPC(oneToFifteen, IBPCx_PASSCOUNT_8͞, passcount);
    🔧IBCC(oneToFifteen, IBCCx_CBE, 0b1);
}

MACRO
void
SimpleDataBreakpoint(
    unsigned int oneToFifteen,
    __builtin_uint_t * addr
)
{
    if (oneToFifteen >= DataBrk()) { return; }
    🔧DBA(oneToFifteen, 0xFFFFFFFF, (__builtin_uint_t)addr);
    🔧DBM(oneToFifteen, 0xFFFFFFFF, 0xFFFFFFFF);
    //🔧DBC(oneToFifteen, IBCx_TE, useAsTriggerPoint ? 0b1 : 0b0);
    //🔧DBC(oneToFifteen, IBCx_BE, 0b1);
}

// A tuple breakpoint is the logical AND of a data breakpoint and an instruction
// breakpoint.

/*
 
 The on-chip SRAM or trace memory is written continuously as a circular buffer.
 It is accessible via drseg address mapped registers. There are registers for
 the read pointer, write pointer, and trace word. The write pointer register
 includes a wrap bit that indicates that the pointer has wrapped since the last
 time the register was written. Before starting trace, the write pointer
 would typically be set to 0. To read the trace memory, the read pointer should
 be set to 0 if there has not been a wrap, or to the value of the write pointer
 if there has been. Reading the trace word register will read the entry pointed
 to by the read pointer and will automatically increment the read pointer.
 Software can continue reading until all valid entries have been read out.
 
 */

MACRO void NextiFlowTraceWord(__builtin_uint_t *lo, __builtin_uint_t *hi) {
    if (ITCBWRP_Wrap & 🔎(ITCBWRP))  { 🔧(ITCBRDP, ITCBWRP_ADDR_3͞1͞, 🔎(ITCBWRP)); } else { 🔧(ITCBRDP) = 0;  }
    *lo = 🔎(ITCBTW_LO); *hi = 🔎(ITCBTW_HI);
}

MACRO __builtin_uint_t * LastiFlowTraceWord() { return (__builtin_uint_t *)🔎(ITCBWRP); }

/**
 
 Enable/disable single-stepping the code. When single-step mode is enabled, a
 Debug Single Step exception (EJTAG_debug) occurs each time the processor has
 taken a single execution step in Non-Debug Mode.
 
 */

MACRO void SingleStep(bool enable) { if (HasSingleStep()) { 🔧Debug(DEBUG_SST, enable ? 1 : 0); } }

/**  Append user-defined value to the trace. */

MACRO void AppendToTrace(uint32_t n) { if (HasiFlowTrace()) { 🔧UserTraceData1(n); } }

/**  Stop outputting the trace. */

MACRO void StopTrace() { if (HasiFlowTrace()) { 🔧(IFCTL, IFCTL_ON, 0b0); } }

/**
 
 Output a textual representation of a value in a memory cell, given it is
 stored at @c address.
 
 */

extern "C"
void
Disassemble(
    __builtin_uint_t address,
    __builtin_uint_t value
);

#define SDBBP(code) /* asm volatile("sdbbp " #code); */ asm volatile("mfc0 $8, $23, 0; or $8, 0x40000000; mtc0 $8, $23, 0");
#define BREAK(code) asm volatile("break " #code);

#endif
