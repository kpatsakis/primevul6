tputs(const char *string, int affcnt GCC_UNUSED, int (*outc) (int) GCC_UNUSED)
/* stub tputs() that dumps sequences in a visible form */
{
    if (profiling)
	xmits += strlen(string);
    else
	(void) fputs(_nc_visbuf(string), stdout);
    return (OK);
}