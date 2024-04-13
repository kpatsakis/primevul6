uc_err uc_query(uc_engine *uc, uc_query_type type, size_t *result)
{
    UC_INIT(uc);

    switch (type) {
    default:
        return UC_ERR_ARG;

    case UC_QUERY_PAGE_SIZE:
        *result = uc->target_page_size;
        break;

    case UC_QUERY_ARCH:
        *result = uc->arch;
        break;

    case UC_QUERY_MODE:
#ifdef UNICORN_HAS_ARM
        if (uc->arch == UC_ARCH_ARM) {
            return uc->query(uc, type, result);
        }
#endif
        *result = uc->mode;
        break;

    case UC_QUERY_TIMEOUT:
        *result = uc->timed_out;
        break;
    }

    return UC_ERR_OK;
}