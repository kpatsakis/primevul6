vgr_load_dummy_buf(
	char_u *fname,
	char_u *dirname_start,
	char_u *dirname_now)
{
    int		save_mls;
#if defined(FEAT_SYN_HL)
    char_u	*save_ei = NULL;
#endif
    buf_T	*buf;

#if defined(FEAT_SYN_HL)
    // Don't do Filetype autocommands to avoid loading syntax and
    // indent scripts, a great speed improvement.
    save_ei = au_event_disable(",Filetype");
#endif
    // Don't use modelines here, it's useless.
    save_mls = p_mls;
    p_mls = 0;

    // Load file into a buffer, so that 'fileencoding' is detected,
    // autocommands applied, etc.
    buf = load_dummy_buffer(fname, dirname_start, dirname_now);

    p_mls = save_mls;
#if defined(FEAT_SYN_HL)
    au_event_restore(save_ei);
#endif

    return buf;
}