static bool tb_cmp(struct uc_struct *uc, const void *ap, const void *bp)
{
    const TranslationBlock *a = ap;
    const TranslationBlock *b = bp;

    return a->pc == b->pc &&
        a->cs_base == b->cs_base &&
        a->flags == b->flags &&
        (tb_cflags(a) & CF_HASH_MASK) == (tb_cflags(b) & CF_HASH_MASK) &&
        a->trace_vcpu_dstate == b->trace_vcpu_dstate &&
        a->page_addr[0] == b->page_addr[0] &&
        a->page_addr[1] == b->page_addr[1];
}