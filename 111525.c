uc_err uc_reg_write_batch(uc_engine *uc, int *ids, void *const *vals, int count)
{
    int ret = UC_ERR_OK;

    UC_INIT(uc);

    if (uc->reg_write) {
        ret = uc->reg_write(uc, (unsigned int *)ids, vals, count);
    } else {
        return UC_ERR_HANDLE;
    }

    return ret;
}