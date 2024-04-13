static void do_tb_flush(CPUState *cpu, run_on_cpu_data tb_flush_count)
{
    mmap_lock();
    /* If it is already been done on request of another CPU,
     * just retry.
     */
    if (cpu->uc->tcg_ctx->tb_ctx.tb_flush_count != tb_flush_count.host_int) {
        goto done;
    }

#if 0
    if (DEBUG_TB_FLUSH_GATE) {
        size_t nb_tbs = tcg_nb_tbs(cpu->uc->tcg_ctx);
        size_t host_size = 0;

        tcg_tb_foreach(cpu->uc->tcg_ctx, tb_host_size_iter, &host_size);
        //printf("qemu: flush code_size=%zu nb_tbs=%zu avg_tb_size=%zu\n",
        //       tcg_code_size(cpu->uc->tcg_ctx), nb_tbs, nb_tbs > 0 ? host_size / nb_tbs : 0);
    }

    CPU_FOREACH(cpu) {
        cpu_tb_jmp_cache_clear(cpu);
    }
#else
    cpu_tb_jmp_cache_clear(cpu);
#endif

    qht_reset_size(cpu->uc, &cpu->uc->tcg_ctx->tb_ctx.htable, CODE_GEN_HTABLE_SIZE);
    page_flush_tb(cpu->uc);

    tcg_region_reset_all(cpu->uc->tcg_ctx);
    /* XXX: flush processor icache at this point if cache flush is
       expensive */
    cpu->uc->tcg_ctx->tb_ctx.tb_flush_count = cpu->uc->tcg_ctx->tb_ctx.tb_flush_count + 1;

done:
    mmap_unlock();
}