void tb_check_watchpoint(CPUState *cpu, uintptr_t retaddr)
{
    TCGContext *tcg_ctx = cpu->uc->tcg_ctx;
    TranslationBlock *tb;

    assert_memory_lock();

    tb = tcg_tb_lookup(tcg_ctx, retaddr);
    if (tb) {
        /* We can use retranslation to find the PC.  */
        cpu_restore_state_from_tb(cpu, tb, retaddr, true);
        tb_phys_invalidate(tcg_ctx, tb, -1);
    } else {
        /* The exception probably happened in a helper.  The CPU state should
           have been saved before calling it. Fetch the PC from there.  */
        CPUArchState *env = cpu->env_ptr;
        target_ulong pc, cs_base;
        tb_page_addr_t addr;
        uint32_t flags;

        cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);
        addr = get_page_addr_code(env, pc);
        if (addr != -1) {
            tb_invalidate_phys_range(cpu->uc, addr, addr + 1);
        }
    }
}