static void tb_htable_init(struct uc_struct *uc)
{
    unsigned int mode = QHT_MODE_AUTO_RESIZE;

    qht_init(&uc->tcg_ctx->tb_ctx.htable, tb_cmp, CODE_GEN_HTABLE_SIZE, mode);
}