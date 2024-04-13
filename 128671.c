textw_text_set_cache(gs_text_enum_t *pte, const double *pw,
                   gs_text_cache_control_t control)
{
    textw_text_enum_t *const penum = (textw_text_enum_t *)pte;

    switch (control) {
        case TEXT_SET_CHAR_WIDTH:
        case TEXT_SET_CACHE_DEVICE:
            return gs_text_set_cache(pte, pw, control);
        case TEXT_SET_CACHE_DEVICE2:
            if (penum->cdevproc_callout) {
                memcpy(penum->cdevproc_result, pw, sizeof(penum->cdevproc_result));
                return 0;
            }
            return gs_text_set_cache(pte, pw, control);
        default:
            return_error(gs_error_rangecheck);
    }
    return 0;
}