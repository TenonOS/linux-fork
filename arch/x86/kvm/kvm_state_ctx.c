#include "kvm_state_ctx.h"
#include <linux/slab.h>
#include <linux/kernel.h>

static void copy_memslot_info_to_ept_ctx(kvm_state_ctx *ctx, int instance_id, int slot_id, struct kvm_memory_slot *origin_slot) {
    ctx->ept_ctx.userspace_memslots[instance_id][slot_id].slot = (origin_slot->as_id << 16) + origin_slot->id;
    ctx->ept_ctx.userspace_memslots[instance_id][slot_id].flags = origin_slot->flags;
    ctx->ept_ctx.userspace_memslots[instance_id][slot_id].guest_phys_addr = origin_slot->base_gfn << PAGE_SHIFT;
    ctx->ept_ctx.userspace_memslots[instance_id][slot_id].memory_size = origin_slot->npages << PAGE_SHIFT;
    ctx->ept_ctx.userspace_memslots[instance_id][slot_id].userspace_addr = origin_slot->userspace_addr;
}

void init_kvm_ept_ctx(kvm_state_ctx *ctx, int instance_num, int per_instance_slots_num) {
    ctx->ept_ctx.instance_num = instance_num;
    ctx->ept_ctx.per_instance_slots = per_instance_slots_num;
    ctx->ept_ctx.userspace_memslots = (struct kvm_userspace_memory_region **)kzalloc(sizeof(struct kvm_userspace_memory_region *) * ctx->ept_ctx.instance_num, GFP_KERNEL);
    int i = 0;
    for (; i < ctx->ept_ctx.instance_num; i++) {
        ctx->ept_ctx.userspace_memslots[i] = kzalloc(sizeof(struct kvm_userspace_memory_region) * ctx->ept_ctx.per_instance_slots, GFP_KERNEL);
    }
}

void save_kvm_userspace_memslot_mapping(kvm_state_ctx *ctx, struct kvm_vcpu *vcpu) {
    struct kvm *kvm_instance = vcpu->kvm;
    int instance_num = 2;                     // Fixed value in x86 implement
    int per_instance_slot_num = 10;
    init_kvm_ept_ctx(ctx, instance_num, per_instance_slot_num);
    int instance_id = 0;
    for (; instance_id < instance_num; instance_id++) {
        int bkt;
        struct kvm_memory_slot *memslot;
        struct kvm_memslots *slots = kvm_instance->memslots[instance_id];
        hash_for_each(slots->id_hash, bkt, memslot, id_node[1])
            copy_memslot_info_to_ept_ctx(ctx, instance_id, bkt, memslot);
    }
}

void printk_show_memory_slot_in_userspace_mem(kvm_state_ctx *ctx) {
    kvm_ept_ctx *ept_ctx = &ctx->ept_ctx;
    int instance_id = 0;
    for (; instance_id < ept_ctx->instance_num; instance_id++) {
        int slot_id = 0;
        for (; slot_id < ept_ctx->per_instance_slots; slot_id++) {
            struct kvm_userspace_memory_region *kvm_mem_slot = &ept_ctx->userspace_memslots[instance_id][slot_id];
            printk(KERN_INFO "slot:%d flags:%x guest_phys_addr:%llx mem_size:%lld userspace_addr:%llx\n", kvm_mem_slot->slot, kvm_mem_slot->flags, 
	               kvm_mem_slot->userspace_addr, kvm_mem_slot->memory_size, kvm_mem_slot->guest_phys_addr);
        }
    }
}