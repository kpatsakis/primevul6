uc_err uc_context_restore(uc_engine *uc, uc_context *context)
{
    UC_INIT(uc);

    memcpy(uc->cpu->env_ptr, context->data, context->context_size);

    return UC_ERR_OK;
}