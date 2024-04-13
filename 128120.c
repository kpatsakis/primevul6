iemsg(char *s)
{
    if (!emsg_not_now())
    {
	emsg_core((char_u *)s);
#if defined(ABORT_ON_INTERNAL_ERROR) && defined(FEAT_EVAL)
	set_vim_var_string(VV_ERRMSG, (char_u *)s, -1);
	abort();
#endif
    }
}