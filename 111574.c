uc_err uc_reg_write(uc_engine *uc, int regid, const void *value)
{
    UC_INIT(uc);
    return uc_reg_write_batch(uc, &regid, (void *const *)&value, 1);
}