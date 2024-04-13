TranslationBlock *tb_gen_code(CPUState *cpu,
                              target_ulong pc, target_ulong cs_base,
                              uint32_t flags, int cflags)
{
#ifdef TARGET_ARM
    struct uc_struct *uc = cpu->uc;
#endif
    TCGContext *tcg_ctx = cpu->uc->tcg_ctx;
    CPUArchState *env = cpu->env_ptr;
    TranslationBlock *tb, *existing_tb;
    tb_page_addr_t phys_pc, phys_page2;
    target_ulong virt_page2;
    tcg_insn_unit *gen_code_buf;
    int gen_code_size, search_size, max_insns;

    assert_memory_lock();
#ifdef HAVE_PTHREAD_JIT_PROTECT
    tb_exec_unlock(tcg_ctx);
#endif
    phys_pc = get_page_addr_code(env, pc);

    if (phys_pc == -1) {
        /* Generate a temporary TB with 1 insn in it */
        cflags &= ~CF_COUNT_MASK;
        cflags |= CF_NOCACHE | 1;
    }

    cflags &= ~CF_CLUSTER_MASK;
    cflags |= cpu->cluster_index << CF_CLUSTER_SHIFT;

    max_insns = cflags & CF_COUNT_MASK;
    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }
    if (max_insns > TCG_MAX_INSNS) {
        max_insns = TCG_MAX_INSNS;
    }
    if (cpu->singlestep_enabled) {
        max_insns = 1;
    }

 buffer_overflow:
    tb = tcg_tb_alloc(tcg_ctx);
    if (unlikely(!tb)) {
        /* flush must be done */
        tb_flush(cpu);
        mmap_unlock();
        /* Make the execution loop process the flush as soon as possible.  */
        cpu->exception_index = EXCP_INTERRUPT;
        cpu_loop_exit(cpu);
    }

    gen_code_buf = tcg_ctx->code_gen_ptr;
    tb->tc.ptr = gen_code_buf;
    tb->pc = pc;
    tb->cs_base = cs_base;
    tb->flags = flags;
    tb->cflags = cflags;
    tb->orig_tb = NULL;
    tb->trace_vcpu_dstate = *cpu->trace_dstate;
    tcg_ctx->tb_cflags = cflags;
 tb_overflow:

    tcg_func_start(tcg_ctx);

    tcg_ctx->cpu = env_cpu(env);
    gen_intermediate_code(cpu, tb, max_insns);
    tcg_ctx->cpu = NULL;

    /* generate machine code */
    tb->jmp_reset_offset[0] = TB_JMP_RESET_OFFSET_INVALID;
    tb->jmp_reset_offset[1] = TB_JMP_RESET_OFFSET_INVALID;
    tcg_ctx->tb_jmp_reset_offset = tb->jmp_reset_offset;
    if (TCG_TARGET_HAS_direct_jump) {
        tcg_ctx->tb_jmp_insn_offset = tb->jmp_target_arg;
        tcg_ctx->tb_jmp_target_addr = NULL;
    } else {
        tcg_ctx->tb_jmp_insn_offset = NULL;
        tcg_ctx->tb_jmp_target_addr = tb->jmp_target_arg;
    }

    gen_code_size = tcg_gen_code(tcg_ctx, tb);
    if (unlikely(gen_code_size < 0)) {
        switch (gen_code_size) {
        case -1:
            /*
             * Overflow of code_gen_buffer, or the current slice of it.
             *
             * TODO: We don't need to re-do gen_intermediate_code, nor
             * should we re-do the tcg optimization currently hidden
             * inside tcg_gen_code.  All that should be required is to
             * flush the TBs, allocate a new TB, re-initialize it per
             * above, and re-do the actual code generation.
             */
            goto buffer_overflow;

        case -2:
            /*
             * The code generated for the TranslationBlock is too large.
             * The maximum size allowed by the unwind info is 64k.
             * There may be stricter constraints from relocations
             * in the tcg backend.
             *
             * Try again with half as many insns as we attempted this time.
             * If a single insn overflows, there's a bug somewhere...
             */
            max_insns = tb->icount;
            assert(max_insns > 1);
            max_insns /= 2;
            goto tb_overflow;

        default:
            g_assert_not_reached();
        }
    }
    search_size = encode_search(cpu->uc, tb, (uint8_t *)gen_code_buf + gen_code_size);
    if (unlikely(search_size < 0)) {
        goto buffer_overflow;
    }
    tb->tc.size = gen_code_size;

    tcg_ctx->code_gen_ptr = (void *)
        ROUND_UP((uintptr_t)gen_code_buf + gen_code_size + search_size,
                 CODE_GEN_ALIGN);

    /* init jump list */
    tb->jmp_list_head = (uintptr_t)NULL;
    tb->jmp_list_next[0] = (uintptr_t)NULL;
    tb->jmp_list_next[1] = (uintptr_t)NULL;
    tb->jmp_dest[0] = (uintptr_t)NULL;
    tb->jmp_dest[1] = (uintptr_t)NULL;

    /* init original jump addresses which have been set during tcg_gen_code() */
    if (tb->jmp_reset_offset[0] != TB_JMP_RESET_OFFSET_INVALID) {
        tb_reset_jump(tb, 0);
    }
    if (tb->jmp_reset_offset[1] != TB_JMP_RESET_OFFSET_INVALID) {
        tb_reset_jump(tb, 1);
    }

    /* check next page if needed */
    virt_page2 = (pc + tb->size - 1) & TARGET_PAGE_MASK;
    phys_page2 = -1;
    if ((pc & TARGET_PAGE_MASK) != virt_page2) {
        phys_page2 = get_page_addr_code(env, virt_page2);
    }
    /*
     * No explicit memory barrier is required -- tb_link_page() makes the
     * TB visible in a consistent state.
     */
    existing_tb = tb_link_page(cpu->uc, tb, phys_pc, phys_page2);
    /* if the TB already exists, discard what we just translated */
    if (unlikely(existing_tb != tb)) {
        uintptr_t orig_aligned = (uintptr_t)gen_code_buf;

        orig_aligned -= ROUND_UP(sizeof(*tb), tcg_ctx->uc->qemu_icache_linesize);
        tcg_ctx->code_gen_ptr = (void *)orig_aligned;
        return existing_tb;
    }
    tcg_tb_insert(tcg_ctx, tb);
    return tb;
}