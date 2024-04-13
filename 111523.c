static void tb_jmp_cache_clear_page(CPUState *cpu, target_ulong page_addr)
{
    unsigned int i, i0 = tb_jmp_cache_hash_page(cpu->uc, page_addr);

    for (i = 0; i < TB_JMP_PAGE_SIZE; i++) {
        cpu->tb_jmp_cache[i0 + i] = NULL;
    }
}