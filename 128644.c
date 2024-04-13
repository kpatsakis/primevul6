build_gs_primitive_font(i_ctx_t *i_ctx_p, os_ptr op, gs_font_base ** ppfont,
                        font_type ftype, gs_memory_type_ptr_t pstype,
                        const build_proc_refs * pbuild,
                        build_font_options_t options)
{
    ref *pcharstrings = 0;
    ref CharStrings;
    gs_font_base *pfont;
    font_data *pdata;
    int code;

    if (dict_find_string(op, "CharStrings", &pcharstrings) <= 0) {
        if (!(options & bf_CharStrings_optional))
            return_error(gs_error_invalidfont);
    } else {
        ref *ignore;

        if (!r_has_type(pcharstrings, t_dictionary))
            return_error(gs_error_invalidfont);
        if ((options & bf_notdef_required) != 0 &&
            dict_find_string(pcharstrings, ".notdef", &ignore) <= 0
            )
            return_error(gs_error_invalidfont);
        /*
         * Since build_gs_simple_font may resize the dictionary and cause
         * pointers to become invalid, save CharStrings.
         */
        CharStrings = *pcharstrings;
    }
    code = build_gs_outline_font(i_ctx_p, op, ppfont, ftype, pstype, pbuild,
                                 options, build_gs_simple_font);
    if (code != 0)
        return code;
    pfont = *ppfont;
    pdata = pfont_data(pfont);
    if (pcharstrings)
        ref_assign(&pdata->CharStrings, &CharStrings);
    else
        make_null(&pdata->CharStrings);
    /* Check that the UniqueIDs match.  This is part of the */
    /* Adobe protection scheme, but we may as well emulate it. */
    if (uid_is_valid(&pfont->UID) &&
        !dict_check_uid_param(op, &pfont->UID)
        )
        uid_set_invalid(&pfont->UID);
    if (uid_is_valid(&pfont->UID)) {
        const gs_font *pfont0 = (const gs_font *)pfont;

        code = gs_font_find_similar(ifont_dir, &pfont0,
                       font_with_same_UID_and_another_metrics);
        if (code < 0)
            return code; /* Must not happen. */
        if (code)
            uid_set_invalid(&pfont->UID);
    }
    return 0;
}