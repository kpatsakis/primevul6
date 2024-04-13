static void uc_invalidate_tb(struct uc_struct *uc, uint64_t start_addr, size_t len) 
{
    tb_page_addr_t start, end;

    // GVA to GPA 
    // (GPA -> HVA via memory_region_get_ram_addr(mr) + GPA + block->host,
    // HVA->HPA via host mmu)
    start = get_page_addr_code(uc->cpu->env_ptr, start_addr) & (target_ulong)(-1);
    
    // For 32bit target.
    end = (start + len) & (target_ulong)(-1);

    // We get a wrap?
    if (start > end) {
        return;
    }

    tb_invalidate_phys_range(uc, start, end);
}