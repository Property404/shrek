.global get_cpsr
.global set_cpsr
.global enable_interrupts
.global disable_interrupts

get_cpsr:
    push {v1, lr}
    mrs r0, cpsr
    pop {v1, pc}

set_cpsr:
    push {v1, lr}
    msr cpsr, r0
    pop {v1, pc}

enable_interrupts:
    push {v1, lr}
    mrs v1, cpsr
    bic v1, v1, #0x80
    msr cpsr, v1
    pop {v1, pc}

disable_interrupts:
    push {v1, lr}
    mrs r0, cpsr
    orr r0, #7
    msr cpsr, r0
    pop {v1, pc}
