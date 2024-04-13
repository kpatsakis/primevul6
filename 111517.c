static uc_err uc_init(uc_engine *uc)
{

    if (uc->init_done) {
        return UC_ERR_HANDLE;
    }

    uc->exits = g_tree_new_full(uc_exits_cmp, NULL, g_free, NULL);

    if (machine_initialize(uc)) {
        return UC_ERR_RESOURCE;
    }

    // init fpu softfloat
    uc->softfloat_initialize();

    if (uc->reg_reset) {
        uc->reg_reset(uc);
    }

    uc->init_done = true;

    return UC_ERR_OK;
}