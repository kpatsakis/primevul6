uc_err uc_context_reg_write_batch(uc_context *ctx, int *ids, void *const *vals,
                                  int count)
{
    int ret = UC_ERR_OK;
    context_reg_rw_t rw;

    find_context_reg_rw_function(ctx->arch, ctx->mode, &rw);
    if (rw.context_reg_write) {
        ret = rw.context_reg_write(ctx, (unsigned int *)ids, vals, count);
    } else {
        return UC_ERR_HANDLE;
    }

    return ret;
}