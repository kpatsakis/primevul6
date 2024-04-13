bool cpu_restore_state(CPUState *cpu, uintptr_t host_pc, bool will_exit)
{
    TCGContext *tcg_ctx = cpu->uc->tcg_ctx;
    TranslationBlock *tb;
    bool r = false;
    uintptr_t check_offset;
    struct uc_struct *uc = cpu->uc;

    /* The host_pc has to be in the region of current code buffer. If
     * it is not we will not be able to resolve it here. The two cases
     * where host_pc will not be correct are:
     *
     *  - fault during translation (instruction fetch)
     *  - fault from helper (not using GETPC() macro)
     *
     * Either way we need return early as we can't resolve it here.
     *
     * We are using unsigned arithmetic so if host_pc <
     * tcg_init_ctx.code_gen_buffer check_offset will wrap to way
     * above the code_gen_buffer_size
     */
    check_offset = host_pc - (uintptr_t) uc->tcg_ctx->code_gen_buffer;

    if (check_offset < uc->tcg_ctx->code_gen_buffer_size) {
        tb = tcg_tb_lookup(tcg_ctx, host_pc);
        if (tb) {
            cpu_restore_state_from_tb(cpu, tb, host_pc, will_exit);
            if (tb_cflags(tb) & CF_NOCACHE) {
                /* one-shot translation, invalidate it immediately */
                tb_phys_invalidate(tcg_ctx, tb, -1);
                tcg_tb_remove(tcg_ctx, tb);
            }
            r = true;
        }
    }

    return r;
}