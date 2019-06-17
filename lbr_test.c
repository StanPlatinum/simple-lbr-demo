
#include <stdio.h>

#include "msr.h"
#define IA32_DEBUGCTLMSR_LBR		(1<<0) /* Last Branch Record */

/*Skylake add*/
#define MSR_LBR_SELECT              0x000001c8
#define SELECT_CONFIG               0x18D                   //110001101

void lbr_enable(void *info) {
	/*uint64_t debugctl;
	  unsigned long long select;
	  rdmsrl(MSR_IA32_DEBUGCTLMSR, debugctl);
	  debugctl |= (IA32_DEBUGCTLMSR_LBR );
	  wrmsrl(MSR_IA32_DEBUGCTLMSR, debugctl);*/
	//printk("intel lbr enable..\n");
	unsigned long long select;
	asm volatile    (
			"xor %%edx, %%edx;"
			"xor %%eax, %%eax;"
			"inc %%eax;"
			"mov $0x1d9, %%ecx;"
			"wrmsr;"
			:
			:
			);

	rdmsrl(MSR_LBR_SELECT,select);
	//printk("SELECT 0x%llx", select);
	wrmsrl(MSR_LBR_SELECT, SELECT_CONFIG);
}
