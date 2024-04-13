set_keep_msg(char_u *s, int attr)
{
    vim_free(keep_msg);
    if (s != NULL && msg_silent == 0)
	keep_msg = vim_strsave(s);
    else
	keep_msg = NULL;
    keep_msg_more = FALSE;
    keep_msg_attr = attr;
}