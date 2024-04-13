tb_invalidate_phys_page_range__locked(struct uc_struct *uc, struct page_collection *pages,
                                      PageDesc *p, tb_page_addr_t start,
                                      tb_page_addr_t end,
                                      uintptr_t retaddr)
{
    TranslationBlock *tb;
    tb_page_addr_t tb_start, tb_end;
    int n;
#ifdef TARGET_HAS_PRECISE_SMC
    CPUState *cpu = uc->cpu;
    CPUArchState *env = NULL;
    bool current_tb_not_found = retaddr != 0;
    bool current_tb_modified = false;
    TranslationBlock *current_tb = NULL;
    target_ulong current_pc = 0;
    target_ulong current_cs_base = 0;
    uint32_t current_flags = 0;
#endif /* TARGET_HAS_PRECISE_SMC */

    assert_page_locked(p);

#if defined(TARGET_HAS_PRECISE_SMC)
    if (cpu != NULL) {
        env = cpu->env_ptr;
    }
#endif

    /* we remove all the TBs in the range [start, end[ */
    /* XXX: see if in some cases it could be faster to invalidate all
       the code */
    PAGE_FOR_EACH_TB(p, tb, n) {
        assert_page_locked(p);
        /* NOTE: this is subtle as a TB may span two physical pages */
        if (n == 0) {
            /* NOTE: tb_end may be after the end of the page, but
               it is not a problem */
            tb_start = tb->page_addr[0] + (tb->pc & ~TARGET_PAGE_MASK);
            tb_end = tb_start + tb->size;
        } else {
            tb_start = tb->page_addr[1];
            tb_end = tb_start + ((tb->pc + tb->size) & ~TARGET_PAGE_MASK);
        }
        // Unicorn: We may indeed generate a TB without any instruction which breaks qemu assumption.
        if ( (!(tb_end <= start || tb_start >= end)) || (tb_start == tb_end) ) {
#ifdef TARGET_HAS_PRECISE_SMC
            if (current_tb_not_found) {
                current_tb_not_found = false;
                /* now we have a real cpu fault */
                current_tb = tcg_tb_lookup(uc->tcg_ctx, retaddr);
            }
            if (current_tb == tb &&
                (tb_cflags(current_tb) & CF_COUNT_MASK) != 1) {
                /*
                 * If we are modifying the current TB, we must stop
                 * its execution. We could be more precise by checking
                 * that the modification is after the current PC, but it
                 * would require a specialized function to partially
                 * restore the CPU state.
                 */
                current_tb_modified = true;
                cpu_restore_state_from_tb(cpu, current_tb, retaddr, true);
                cpu_get_tb_cpu_state(env, &current_pc, &current_cs_base,
                                     &current_flags);
            }
#endif /* TARGET_HAS_PRECISE_SMC */
            tb_phys_invalidate__locked(uc->tcg_ctx, tb);
        }
    }

    /* if no code remaining, no need to continue to use slow writes */
    if (!p->first_tb) {
        invalidate_page_bitmap(p);
        tlb_unprotect_code(uc, start);
    }

#ifdef TARGET_HAS_PRECISE_SMC
    if (current_tb_modified) {
        page_collection_unlock(pages);
        /* Force execution of one insn next time.  */
        cpu->cflags_next_tb = 1 | curr_cflags();
        mmap_unlock();
        cpu_loop_exit_noexc(cpu);
    }
#endif
}