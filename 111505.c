size_t uc_context_size(uc_engine *uc)
{
    UC_INIT(uc);
    // return the total size of struct uc_context
    return sizeof(uc_context) + uc->cpu_context_size;
}