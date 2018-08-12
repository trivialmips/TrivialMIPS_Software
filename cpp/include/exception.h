#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

const char* EXCEPTION_MESSAGES[32] = {
    "Interrupt",
    "TLB modification exception",
    "TLB exception (load or instruction fetch)",
    "TLB exception (store)",
    "Address error exception (load or instruction fetch)",
    "Address error exception (store)",
    "Bus error exception (instruction fetch)",
    "Bus error exception (data reference: load or store)",
    "Syscall exception",
    "Breakpoint exception",
    "Reserved instruction exception",
    "Coprocessor Unusable exception",
    "Arithmetic Overflow exception",
    "Trap exception",
    "MSA Floating-Point exception",
    "Floating-Point exception",
    "",
    "",
    "",
    "TLB Read-Inhibit exception",
    "TLB Execution-Inhibit exception",
    "",
    "",
    "WATCH",
    "Machine check",
    "Thread Allocation, Deallocation, or Scheduling Exceptions",
    "",
    "",
    "",
    "",
    "Cache error",
    ""
};

#endif