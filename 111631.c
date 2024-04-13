void tcg_flush_softmmu_tlb(struct uc_struct *uc)
{
    tlb_flush(uc->cpu);
}