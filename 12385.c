nv_hat(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
	(void)buflist_getfile((int)cap->count0, (linenr_T)0,
						GETF_SETMARK|GETF_ALT, FALSE);
}