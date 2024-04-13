uc_err uc_context_reg_write(uc_context *ctx, int regid, const void *value)
{
    return uc_context_reg_write_batch(ctx, &regid, (void *const *)&value, 1);
}