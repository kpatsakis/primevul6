uc_err uc_reg_read_batch(uc_engine *uc, int *ids, void **vals, int count)
{
    int ret = UC_ERR_OK;

    UC_INIT(uc);

    if (uc->reg_read) {
        ret = uc->reg_read(uc, (unsigned int *)ids, vals, count);
    } else {
        return UC_ERR_HANDLE;
    }

    return ret;
}