cexpr_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	case CMD_cexpr:	    return (char_u *)"cexpr";
	case CMD_cgetexpr:  return (char_u *)"cgetexpr";
	case CMD_caddexpr:  return (char_u *)"caddexpr";
	case CMD_lexpr:	    return (char_u *)"lexpr";
	case CMD_lgetexpr:  return (char_u *)"lgetexpr";
	case CMD_laddexpr:  return (char_u *)"laddexpr";
	default:	    return NULL;
    }
}