uc_err uc_context_reg_read(uc_context *ctx, int regid, void *value)
{
    return uc_context_reg_read_batch(ctx, &regid, &value, 1);
}