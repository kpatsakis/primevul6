cfile_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	case CMD_cfile:	    return (char_u *)"cfile";
	case CMD_cgetfile:  return (char_u *)"cgetfile";
	case CMD_caddfile:  return (char_u *)"caddfile";
	case CMD_lfile:	    return (char_u *)"lfile";
	case CMD_lgetfile:  return (char_u *)"lgetfile";
	case CMD_laddfile:  return (char_u *)"laddfile";
	default:	    return NULL;
    }
}