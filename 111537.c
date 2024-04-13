uc_err uc_context_alloc(uc_engine *uc, uc_context **context)
{
    struct uc_context **_context = context;
    size_t size = uc_context_size(uc);

    UC_INIT(uc);

    *_context = g_malloc(size);
    if (*_context) {
        (*_context)->context_size = uc->cpu_context_size;
        (*_context)->arch = uc->arch;
        (*_context)->mode = uc->mode;
        return UC_ERR_OK;
    } else {
        return UC_ERR_NOMEM;
    }
}