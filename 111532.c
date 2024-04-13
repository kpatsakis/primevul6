void tcg_exec_init(struct uc_struct *uc, unsigned long tb_size)
{
    /* remove tcg object. init here. */
    /* tcg class init: tcg-all.c:tcg_accel_class_init(), skip all. */
    /* tcg object init: tcg-all.c:tcg_accel_instance_init(), skip all. */
    /* tcg init: tcg-all.c: tcg_init(), skip all. */
    /* run tcg_exec_init() here. */
    uc->tcg_ctx = g_malloc(sizeof(TCGContext));
    tcg_context_init(uc->tcg_ctx);
    uc->tcg_ctx->uc = uc;
    page_init(uc);
    tb_htable_init(uc);
    code_gen_alloc(uc, tb_size);
    tb_exec_unlock(uc->tcg_ctx);
    tcg_prologue_init(uc->tcg_ctx);
    /* cpu_interrupt_handler is not used in uc1 */
    uc->l1_map = g_malloc0(sizeof(void *) * V_L1_MAX_SIZE);
    /* Invalidate / Cache TBs */
    uc->uc_invalidate_tb = uc_invalidate_tb;
    uc->uc_gen_tb = uc_gen_tb;

    /* Inline hooks optimization */
    uc->add_inline_hook = uc_add_inline_hook;
    uc->del_inline_hook = uc_del_inline_hook;
}