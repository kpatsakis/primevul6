trigger_winclosed(win_T *win)
{
    static int	recursive = FALSE;
    char_u	winid[NUMBUFLEN];

    if (recursive)
	return;
    recursive = TRUE;
    vim_snprintf((char *)winid, sizeof(winid), "%i", win->w_id);
    apply_autocmds(EVENT_WINCLOSED, winid, winid, FALSE, win->w_buffer);
    recursive = FALSE;
}