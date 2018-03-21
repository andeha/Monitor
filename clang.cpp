//
//  clang.cpp
//  The MIPS/PIC32 Monitor
//

#include "Monitor.h"

#ifdef  __mips__
typedef unsigned int size_t;
#elif __x86_64__
typedef unsigned long size_t;
#endif
#ifdef  __cplusplus
namespace std { typedef ::size_t size_t; }
#endif

extern "C" void * memset(void * loc, int value, size_t len) {
    char * p = (char *)loc;
    for (unsigned long i = 0; i != len; ++i) { p[i] = value; }
    return loc; }

extern "C" void * memcpy(void * dest, const void * src, size_t n) {
    char *csrc = (char *)src; char *cdest = (char *)dest;
    for (unsigned long i = 0; i < n; i++) cdest[i] = csrc[i];
    return dest; }
	
extern "C" INLINED uint32_t __umoddi3(uint32_t a, uint32_t b) { // a % b   unsigned
    uint32_t val; asm volatile("divu $4, $5; mfhi %0" : "=r" (val)); return val;
}

extern "C" INLINED uint32_t __udivdi3(uint32_t a, uint32_t b) { // a / b   unsigned
    uint32_t val; asm volatile("divu $4, $5; mflo %0" : "=r" (val)); return val;
}

extern "C" void * malloc(size_t);
extern "C" void free(void *);
extern "C" void malloc_stats(void);
void * operator new(std::size_t size) { return malloc(size); }
void * operator new[](std::size_t size) { return malloc(size); }
void operator delete(void * p) throw() { free(p); }
void operator delete[](void * p) throw() { free(p); }
void * _NSConcreteGlobalBlock[32];
void * _NSConcreteStackBlock[32];
// typedef uintptr_t _Unwind_Word;
typedef uint32_t _Unwind_Word;
typedef uint64_t _Unwind_Exception_Class;
typedef enum {
    _URC_NO_REASON = 0,
#if defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__) && \
!defined(__ARM_DWARF_EH__)
    _URC_OK = 0, /* used by ARM EHABI */
#endif
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8,
#if defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__) && \
!defined(__ARM_DWARF_EH__)
    _URC_FAILURE = 9 /* used by ARM EHABI */
#endif
} _Unwind_Reason_Code;
struct _Unwind_Exception;
typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code, _Unwind_Exception *);
struct _Unwind_Exception {
    _Unwind_Exception_Class exception_class;
    _Unwind_Exception_Cleanup_Fn exception_cleanup;
    _Unwind_Word private_1;
    _Unwind_Word private_2;
    /* The Itanium ABI requires that _Unwind_Exception objects are "double-word
     * aligned".  GCC has interpreted this to mean "use the maximum useful
     * alignment for the target"; so do we. */
} __attribute__((__aligned__));
extern "C" void _Unwind_Resume(_Unwind_Exception *exception_object) {}
extern "C" void __cxa_pure_virtual() { /* BLURT("Pure virtual function called"); while (1); */ }
extern "C" void __cxa_guard_release(uint64_t *) {}
extern "C" int __cxa_atexit(void (*fn)(void *), void *arg, void *dso_handle) { return 0; }
extern "C" void __cxa_call_unexpected(void *) { /* terminate(); */ }
// Runtime entry point called by compiler when assigning objects inside copy helper routines
extern "C" void _Block_object_assign(void *destAddr, const void *object, const int flags) {}
// Runtime entry point called by the compiler when disposing of objects inside dispose helper routine
extern "C" void _Block_object_dispose(const void *object, const int flags) {}
struct __byref_x {
    /* header */
    void *isa;
    struct __byref_x *forwarding;
    int flags;
    int size;
    /* no helpers needed */
    int x; };
typedef void (*generic_invoke_funcptr)(void *, ...);
struct __block_literal {
    void *isa;
    int flags;
    int reserved;
    generic_invoke_funcptr invoke;
    struct __block_descriptor_tmp *descriptor;
    struct __byref_x *captured_x; };
extern "C" void __copy_helper_block_(struct __block_literal *dst, struct __block_literal *src) {}
void *__gxx_personality_v0;
