void tb_flush_jmp_cache(CPUState *cpu, target_ulong addr)
{
#ifdef TARGET_ARM
    struct uc_struct *uc = cpu->uc;
#endif

    /* Discard jump cache entries for any tb which might potentially
       overlap the flushed page.  */
    tb_jmp_cache_clear_page(cpu, addr - TARGET_PAGE_SIZE);
    tb_jmp_cache_clear_page(cpu, addr);
}