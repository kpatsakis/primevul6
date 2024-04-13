zbuildfont3(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;
    build_proc_refs build;
    gs_font_base *pfont;

    check_type(*op, t_dictionary);
    code = build_gs_font_procs(op, &build);
    if (code < 0)
        return code;
    code = build_gs_simple_font(i_ctx_p, op, &pfont, ft_user_defined,
                                &st_gs_font_base, &build, bf_options_none);
    if (code < 0)
        return code;
    return define_gs_font(i_ctx_p, (gs_font *) pfont);
}