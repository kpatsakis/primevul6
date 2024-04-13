get_emsg_source(void)
{
    char_u	*Buf, *p;

    if (HAVE_SOURCING_INFO && SOURCING_NAME != NULL && other_sourcing_name())
    {
	char_u	    *sname = estack_sfile(ESTACK_NONE);
	char_u	    *tofree = sname;

	if (sname == NULL)
	    sname = SOURCING_NAME;

#ifdef FEAT_EVAL
	if (estack_compiling)
	    p = (char_u *)_("Error detected while compiling %s:");
	else
#endif
	    p = (char_u *)_("Error detected while processing %s:");
	Buf = alloc(STRLEN(sname) + STRLEN(p));
	if (Buf != NULL)
	    sprintf((char *)Buf, (char *)p, sname);
	vim_free(tofree);
	return Buf;
    }
    return NULL;
}