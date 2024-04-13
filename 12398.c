nv_Zet(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
    {
	switch (cap->nchar)
	{
			// "ZZ": equivalent to ":x".
	    case 'Z':	do_cmdline_cmd((char_u *)"x");
			break;

			// "ZQ": equivalent to ":q!" (Elvis compatible).
	    case 'Q':	do_cmdline_cmd((char_u *)"q!");
			break;

	    default:	clearopbeep(cap->oap);
	}
    }
}