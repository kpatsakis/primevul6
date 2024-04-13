static int cpu_restore_state_from_tb(CPUState *cpu, TranslationBlock *tb,
                                     uintptr_t searched_pc, bool reset_icount)
{
    target_ulong data[TARGET_INSN_START_WORDS] = { tb->pc };
    uintptr_t host_pc = (uintptr_t)tb->tc.ptr;
    CPUArchState *env = cpu->env_ptr;
    uint8_t *p = (uint8_t *)tb->tc.ptr + tb->tc.size;
    int i, j, num_insns = tb->icount;

    searched_pc -= GETPC_ADJ;

    if (searched_pc < host_pc) {
        return -1;
    }

    /* Reconstruct the stored insn data while looking for the point at
       which the end of the insn exceeds the searched_pc.  */
    for (i = 0; i < num_insns; ++i) {
        for (j = 0; j < TARGET_INSN_START_WORDS; ++j) {
            data[j] += decode_sleb128(&p);
        }
        host_pc += decode_sleb128(&p);
        if (host_pc > searched_pc) {
            goto found;
        }
    }
    return -1;

 found:
    if (reset_icount && (tb_cflags(tb) & CF_USE_ICOUNT)) {
        /* Reset the cycle counter to the start of the block
           and shift if to the number of actually executed instructions */
        cpu_neg(cpu)->icount_decr.u16.low += num_insns - i;
    }
    restore_state_to_opc(env, tb, data);

    return 0;
}