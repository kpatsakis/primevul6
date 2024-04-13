void cpu_io_recompile(CPUState *cpu, uintptr_t retaddr)
{
    TCGContext *tcg_ctx = cpu->uc->tcg_ctx;
#if defined(TARGET_MIPS) || defined(TARGET_SH4)
    CPUArchState *env = cpu->env_ptr;
#endif
    TranslationBlock *tb;
    uint32_t n;

    tb = tcg_tb_lookup(tcg_ctx, retaddr);
    if (!tb) {
        cpu_abort(cpu, "cpu_io_recompile: could not find TB for pc=%p",
                  (void *)retaddr);
    }
    cpu_restore_state_from_tb(cpu, tb, retaddr, true);

    /* On MIPS and SH, delay slot instructions can only be restarted if
       they were already the first instruction in the TB.  If this is not
       the first instruction in a TB then re-execute the preceding
       branch.  */
    n = 1;
#if defined(TARGET_MIPS)
    if ((env->hflags & MIPS_HFLAG_BMASK) != 0
        && env->active_tc.PC != tb->pc) {
        env->active_tc.PC -= (env->hflags & MIPS_HFLAG_B16 ? 2 : 4);
        cpu_neg(cpu)->icount_decr.u16.low++;
        env->hflags &= ~MIPS_HFLAG_BMASK;
        n = 2;
    }
#elif defined(TARGET_SH4)
    if ((env->flags & ((DELAY_SLOT | DELAY_SLOT_CONDITIONAL))) != 0
        && env->pc != tb->pc) {
        env->pc -= 2;
        cpu_neg(cpu)->icount_decr.u16.low++;
        env->flags &= ~(DELAY_SLOT | DELAY_SLOT_CONDITIONAL);
        n = 2;
    }
#endif

    /* Generate a new TB executing the I/O insn.  */
    cpu->cflags_next_tb = curr_cflags() | CF_LAST_IO | n;

    if (tb_cflags(tb) & CF_NOCACHE) {
        if (tb->orig_tb) {
            /* Invalidate original TB if this TB was generated in
             * cpu_exec_nocache() */
            tb_phys_invalidate(tcg_ctx, tb->orig_tb, -1);
        }
        tcg_tb_remove(tcg_ctx, tb);
    }

    /* TODO: If env->pc != tb->pc (i.e. the faulting instruction was not
     * the first in the TB) then we end up generating a whole new TB and
     *  repeating the fault, which is horribly inefficient.
     *  Better would be to execute just this insn uncached, or generate a
     *  second new TB.
     */
    cpu_loop_exit_noexc(cpu);
}