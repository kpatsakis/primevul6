nv_help(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
	ex_help(NULL);
}