//
//  flash.cpp
//  Monitor
//

#include "Monitor.h"

#define PIC32MX_DEVCFG0     0xBFC02FFC
#define PIC32MX_DEVID       0xBF80F220
#define PIC32MX_NVMCON      0xBF80F400
#define PIC32MX_NVMCONCLR   0xBF80F404
#define PIC32MX_NVMCONSET   0xBF80F408
#define PIC32MX_NVMADDR     0xBF80F420
#define PIC32MX_NVMDATA     0xBF80F430
#define PIC32MX_BMXPFMSZ    0xBF882060
#define PIC32MX_BMXBOOTSZ   0xBF882070
#define NVMCON_NVMWR        (1 << 15)
#define NVMCON_NVMWREN      (1 << 14)
#define NVMCON_NVMERR       (1 << 13)
#define NVMCON_LVDERR       (1 << 12)
#define PIC32MX_NVMKEY      0xBF80F410
#define NVMKEY1             0xAA996655
#define NVMKEY2             0x556699AA
#define NVMCON_OP_WORD_PROG 0x1
#define NVMCON_OP_PFM_ERASE 0x5

#define PIC32MX_PHYS_PGM_FLASH  0x1D000000
#define PIC32MX_PHYS_BOOT_FLASH 0x1FC00000

__builtin_uint_t Pic32MXDeviceId() { return (🔎(PIC32MX_DEVID) & ~0xF0000000)>>28; }
__builtin_uint_t Pic32MXRevision() { return 🔎(PIC32MX_DEVID) & 0xF0000000; }
__builtin_uint_t Pic32MXBootFlashByteSize() { return 🔎(PIC32MX_BMXBOOTSZ); }
__builtin_uint_t Pic32MXProgramFlashByteSize() { return 🔎(PIC32MX_BMXPFMSZ); }
__builtin_uint_t Pic32MXProgramFlashWritable() { return 🔎(PIC32MX_DEVCFG0) & (0b11111111 << 12); }
bool Pic32MXBootFlashWritable() { return 🔎(PIC32MX_DEVCFG0) & (1 << 24); }
bool Pic32MXCodeProtected() { return 🔎(PIC32MX_DEVCFG0) & (1 << 28); }

#define Virt2Phys(v) ((v) & 0x1FFFFFFF) // Note: These macros only work for KSEG0/KSEG1 addresses.

static
__builtin_uint_t
Pic32MXNvmExec(
    uint32_t op,
    uint32_t timeout
)
{
    🔧(PIC32MX_NVMCON) = NVMCON_NVMWREN | op;
    🔧(PIC32MX_NVMKEY) = NVMKEY1; // Unlock flash registers
    🔧(PIC32MX_NVMKEY) = NVMKEY2;
    🔧(PIC32MX_NVMCONSET) = NVMCON_NVMWR; // Start operation
    while (🔎(PIC32MX_NVMCON) & 0x8000); // Buzy-wait until operation complete
    🔧(PIC32MX_NVMCONCLR) = NVMCON_NVMWREN; // Lock flash registers
    return (PIC32MX_NVMCON & 0x3000); // Return WRERR and LVDERR error status bits
}

__builtin_uint_t Pic32MXEraseEntireProgramFlash() { // Not erasing the boot flash. (Need to use the MTAP to perform a full erase.)
    return Pic32MXNvmExec(NVMCON_OP_PFM_ERASE, 50); }

__builtin_uint_t Pic32MXWriteWord(__builtin_uint_t address, __builtin_uint_t word) {
    🔧(PIC32MX_NVMADDR) = Virt2Phys(address); 🔧(PIC32MX_NVMDATA) = word;
    return Pic32MXNvmExec(NVMCON_OP_WORD_PROG, 5); }
	
//void LockDevice() { if (!Pic32MXCodeProtected()) { Pic32MXWriteWord(DEVCFG0, 0xEFFFFFFF); } }
	
static const struct { // DEVID values as per PIC32MX Flash Programming Specification Rev N.
    uint32_t devid;
    const char * name;
} pic32mxDevs[] = {
    {0x04A07053, "110F016B"}, {0x04A09053, "110F016C"},
    {0x04A0B053, "110F016D"}, {0x04A06053, "120F032B"},
    {0x04A08053, "120F032C"}, {0x04A0A053, "120F032D"},
    {0x04D07053, "130F064B"}, {0x04D09053, "130F064C"},
    {0x04D0B053, "130F064D"}, {0x04D06053, "150F128B"},
    {0x04D08053, "150F128C"}, {0x04D0A053, "150F128D"},
    {0x06610053, "170F256B"}, {0x0661A053, "170F256D"},
    {0x04A01053, "210F016B"}, {0x04A03053, "210F016C"},
    {0x04A05053, "210F016D"}, {0x04A00053, "220F032B"},
    {0x04A02053, "220F032C"}, {0x04A04053, "220F032D"},
    {0x04D01053, "230F064B"}, {0x04D03053, "230F064C"},
    {0x04D05053, "230F064D"}, {0x04D00053, "250F128B"},
    {0x04D02053, "250F128C"}, {0x04D04053, "250F128D"},
    {0x06600053, "270F256B"}, {0x0660A053, "270F256D"},
    {0x05600053, "330F064H"}, {0x05601053, "330F064L"},
    {0x05602053, "430F064H"}, {0x05603053, "430F064L"},
    {0x0570C053, "350F128H"}, {0x0570D053, "350F128L"},
    {0x0570E053, "450F128H"}, {0x0570F053, "450F128L"},
    {0x05704053, "350F256H"}, {0x05705053, "350F256L"},
    {0x05706053, "450F256H"}, {0x05707053, "450F256L"},
    {0x05808053, "370F512H"}, {0x05809053, "370F512L"},
    {0x0580A053, "470F512H"}, {0x0580B053, "470F512L"},
    {0x00938053, "360F512L"}, {0x00934053, "360F256L"},
    {0x0092D053, "340F128L"}, {0x0092A053, "320F128L"},
    {0x00916053, "340F512H"}, {0x00912053, "340F256H"},
    {0x0090D053, "340F128H"}, {0x0090A053, "320F128H"},
    {0x00906053, "320F064H"}, {0x00902053, "320F032H"},
    {0x00978053, "460F512L"}, {0x00974053, "460F256L"},
    {0x0096D053, "440F128L"}, {0x00952053, "440F256H"},
    {0x00956053, "440F512H"}, {0x0094D053, "440F128H"},
    {0x00942053, "420F032H"}, {0x04307053, "795F512L"},
    {0x0430E053, "795F512H"}, {0x04306053, "775F512L"},
    {0x0430D053, "775F512H"}, {0x04312053, "775F256L"},
    {0x04303053, "775F256H"}, {0x04417053, "764F128L"},
    {0x0440B053, "764F128H"}, {0x04341053, "695F512L"},
    {0x04325053, "695F512H"}, {0x04311053, "675F512L"},
    {0x0430C053, "675F512H"}, {0x04305053, "675F256L"},
    {0x0430B053, "675F256H"}, {0x04413053, "664F128L"},
    {0x04407053, "664F128H"}, {0x04411053, "664F064L"},
    {0x04405053, "664F064H"}, {0x0430F053, "575F512L"},
    {0x04309053, "575F512H"}, {0x04333053, "575F256L"},
    {0x04317053, "575F256H"}, {0x0440F053, "564F128L"},
    {0x04403053, "564F128H"}, {0x0440D053, "564F064L"},
    {0x04401053, "564F064H"}, {0x04400053, "534F064H"},
    {0x0440C053, "534F064L"}, {0x00000000, 0 } };

const char * Pic32MXDeviceName() { __builtin_uint_t devId = Pic32MXDeviceId();
    for (int i = 0; pic32mxDevs[i].name != NULL; i++) {
        if (pic32mxDevs[i].devid == (devId & 0x0fffffff)) {
            return pic32mxDevs[i].name; } }
    return NULL; }
