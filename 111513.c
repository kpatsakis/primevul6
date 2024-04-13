uc_err uc_reg_read(uc_engine *uc, int regid, void *value)
{
    UC_INIT(uc);
    return uc_reg_read_batch(uc, &regid, &value, 1);
}