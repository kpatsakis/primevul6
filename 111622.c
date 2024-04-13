static int encode_search(struct uc_struct *uc, TranslationBlock *tb, uint8_t *block)
{
    TCGContext *tcg_ctx = uc->tcg_ctx;
    uint8_t *highwater = tcg_ctx->code_gen_highwater;
    uint8_t *p = block;
    int i, j, n;

    for (i = 0, n = tb->icount; i < n; ++i) {
        target_ulong prev;

        for (j = 0; j < TARGET_INSN_START_WORDS; ++j) {
            if (i == 0) {
                prev = (j == 0 ? tb->pc : 0);
            } else {
                prev = tcg_ctx->gen_insn_data[i - 1][j];
            }
            p = encode_sleb128(p, tcg_ctx->gen_insn_data[i][j] - prev);
        }
        prev = (i == 0 ? 0 : tcg_ctx->gen_insn_end_off[i - 1]);
        p = encode_sleb128(p, tcg_ctx->gen_insn_end_off[i] - prev);

        /* Test for (pending) buffer overflow.  The assumption is that any
           one row beginning below the high water mark cannot overrun
           the buffer completely.  Thus we can test for overflow after
           encoding a row without having to check during encoding.  */
        if (unlikely(p > highwater)) {
            return -1;
        }
    }

    return p - block;
}