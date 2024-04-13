uc_err uc_context_save(uc_engine *uc, uc_context *context)
{
    UC_INIT(uc);

    memcpy(context->data, uc->cpu->env_ptr, context->context_size);

    return UC_ERR_OK;
}