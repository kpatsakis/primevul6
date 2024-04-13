nv_window(cmdarg_T *cap)
{
    if (cap->nchar == ':')
    {
	// "CTRL-W :" is the same as typing ":"; useful in a terminal window
	cap->cmdchar = ':';
	cap->nchar = NUL;
	nv_colon(cap);
    }
    else if (!checkclearop(cap->oap))
	do_window(cap->nchar, cap->count0, NUL); // everything is in window.c
}