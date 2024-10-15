#ifndef KVM_STATE_CTX_H__
#define KVM_STATE_CTX_H__
#include <linux/kvm_host.h>

typedef struct kvm_userspace_mem_map_info {
	struct kvm_userspace_memory_region **userspace_memslots;
	int instance_num;
	int per_instance_slots;                                   // For now we assume each memory instance has the same slots num
} kvm_ept_ctx;

typedef struct kvm_vcpu_state {
    struct kvm_regs regs;
	struct kvm_sregs sregs;
	struct kvm_fpu fpu;
	struct kvm_xsave xsave;
	struct kvm_vcpu_events events;
	struct kvm_lapic_state lapic;
	struct kvm_xcrs xcrs;
	struct kvm_irqchip irqchip[3];
	struct kvm_pit_state2 pit2;
    struct kvm_clock_data user_ns;
	struct kvm_mp_state mp_state;
	struct kvm_debugregs debugregs;
	kvm_ept_ctx ept_ctx;
} kvm_state_ctx ;

void init_kvm_ept_ctx(kvm_state_ctx *ctx, int instance_num, int per_instance_slots_num);

void save_kvm_userspace_memslot_mapping(kvm_state_ctx *ctx, struct kvm_vcpu *vcpu);

void printk_show_memory_slot_in_userspace_mem(kvm_state_ctx *ctx);

#endif