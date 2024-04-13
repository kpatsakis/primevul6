static uc_err uc_gen_tb(struct uc_struct *uc, uint64_t addr, uc_tb *out_tb) 
{
    TranslationBlock *tb;
    target_ulong cs_base, pc;
    CPUState *cpu = uc->cpu;
    CPUArchState *env = (CPUArchState *)cpu->env_ptr;
    uint32_t flags;
    uint32_t hash;
    uint32_t cflags = cpu->cflags_next_tb;

    if (cflags == -1) {
        cflags = curr_cflags();
    }

    cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);

    // Unicorn: Our hack here.
    pc = addr;

    hash = tb_jmp_cache_hash_func(env->uc, pc);
    tb = cpu->tb_jmp_cache[hash];

    cflags &= ~CF_CLUSTER_MASK;
    cflags |= cpu->cluster_index << CF_CLUSTER_SHIFT;

    if (unlikely(!(tb &&
                   tb->pc == pc &&
                   tb->cs_base == cs_base &&
                   tb->flags == flags &&
                   tb->trace_vcpu_dstate == *cpu->trace_dstate &&
                   (tb_cflags(tb) & (CF_HASH_MASK | CF_INVALID)) == cflags))) {

        tb = tb_htable_lookup(cpu, pc, cs_base, flags, cflags);
        cpu->tb_jmp_cache[hash] = tb;

        if (tb == NULL) {
            mmap_lock();
            tb = tb_gen_code(cpu, pc, cs_base, flags, cflags);
            mmap_unlock();
            /* We add the TB in the virtual pc hash table for the fast lookup */
            cpu->tb_jmp_cache[hash] = tb;
        }
    }

    // If we still couldn't generate a TB, it must be out of memory.
    if (tb == NULL) {
        return UC_ERR_NOMEM;
    }

    if (out_tb != NULL) {
        UC_TB_COPY(out_tb, tb);
    }

    return UC_ERR_OK;
}