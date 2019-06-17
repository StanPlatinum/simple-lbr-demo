#ifndef __ASM_MSR_H
#define __ASM_MSR_H

#ifndef __ASSEMBLY__

/* Intel-defined CPU features, CPUID level 0x00000007:0 (ebx), word 7 */
#define X86_FEATURE_FSGSBASE	(7*32+ 0) /* {RD,WR}{FS,GS}BASE instructions */
#define cpu_has_fsgsbase	boot_cpu_has(X86_FEATURE_FSGSBASE)


#ifdef __ASSEMBLY__
# define _ASM_EX(p) p-.
#else
# define _ASM_EX(p) #p "-."
#endif

/* Exception table entry */
#ifdef __ASSEMBLY__
# define _ASM__EXTABLE(sfx, from, to)             \
    .section .ex_table##sfx, "a" ;                \
    .balign 4 ;                                   \
    .long _ASM_EX(from), _ASM_EX(to) ;            \
    .previous
#else
# define _ASM__EXTABLE(sfx, from, to)             \
    " .section .ex_table" #sfx ",\"a\"\n"         \
    " .balign 4\n"                                \
    " .long " _ASM_EX(from) ", " _ASM_EX(to) "\n" \
    " .previous\n"
#endif

#define _ASM_EXTABLE(from, to)     _ASM__EXTABLE(, from, to)


#include <sys/types.h>
#include <sys/errno.h>


typedef unsigned long long __u64;
typedef unsigned long long u64;
typedef unsigned long __u32;
typedef unsigned long u32;

typedef __u64 uint64_t;
typedef __u32 uint32_t;


#define rdmsr(msr,val1,val2) \
     __asm__ __volatile__("rdmsr" \
			  : "=a" (val1), "=d" (val2) \
			  : "c" (msr))

#define rdmsrl(msr,val) do { unsigned long a__,b__; \
       __asm__ __volatile__("rdmsr" \
			    : "=a" (a__), "=d" (b__) \
			    : "c" (msr)); \
       val = a__ | ((u64)b__<<32); \
} while(0)

#define wrmsr(msr,val1,val2) \
     __asm__ __volatile__("wrmsr" \
			  : /* no outputs */ \
			  : "c" (msr), "a" (val1), "d" (val2))

static inline void wrmsrl(unsigned int msr, __u64 val)
{
        __u32 lo, hi;
        lo = (__u32)val;
        hi = (__u32)(val >> 32);
        wrmsr(msr, lo, hi);
}

/* rdmsr with exception handling */
#define rdmsr_safe(msr,val) ({\
    int _rc; \
    uint32_t lo, hi; \
    __asm__ __volatile__( \
        "1: rdmsr\n2:\n" \
        ".section .fixup,\"ax\"\n" \
        "3: xorl %0,%0\n; xorl %1,%1\n" \
        "   movl %5,%2\n; jmp 2b\n" \
        ".previous\n" \
        _ASM_EXTABLE(1b, 3b) \
        : "=a" (lo), "=d" (hi), "=&r" (_rc) \
        : "c" (msr), "2" (0), "i" (-EFAULT)); \
    val = lo | ((uint64_t)hi << 32); \
    _rc; })

/* wrmsr with exception handling */
static inline int wrmsr_safe(unsigned int msr, uint64_t val)
{
    int _rc;
    uint32_t lo, hi;
    lo = (uint32_t)val;
    hi = (uint32_t)(val >> 32);

    __asm__ __volatile__(
        "1: wrmsr\n2:\n"
        ".section .fixup,\"ax\"\n"
        "3: movl %5,%0\n; jmp 2b\n"
        ".previous\n"
        _ASM_EXTABLE(1b, 3b)
        : "=&r" (_rc)
        : "c" (msr), "a" (lo), "d" (hi), "0" (0), "i" (-EFAULT));
    return _rc;
}

static inline uint64_t rdtsc(void)
{
    uint32_t low, high;

    __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));

    return ((uint64_t)high << 32) | low;
}

#define __write_tsc(val) wrmsrl(MSR_IA32_TSC, val)
#define write_tsc(val) ({                                       \
    /* Reliable TSCs are in lockstep across all CPUs. We should \
     * never write to them. */                                  \
    ASSERT(!boot_cpu_has(X86_FEATURE_TSC_RELIABLE));            \
    __write_tsc(val);                                           \
})

#define write_rdtscp_aux(val) wrmsr(MSR_TSC_AUX, (val), 0)

#define rdpmc(counter,low,high) \
     __asm__ __volatile__("rdpmc" \
			  : "=a" (low), "=d" (high) \
			  : "c" (counter))


#endif /* !__ASSEMBLY__ */

#endif /* __ASM_MSR_H */
