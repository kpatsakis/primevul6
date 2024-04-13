make_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	case CMD_make:	    return (char_u *)"make";
	case CMD_lmake:	    return (char_u *)"lmake";
	case CMD_grep:	    return (char_u *)"grep";
	case CMD_lgrep:	    return (char_u *)"lgrep";
	case CMD_grepadd:   return (char_u *)"grepadd";
	case CMD_lgrepadd:  return (char_u *)"lgrepadd";
	default: return NULL;
    }
}