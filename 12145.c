vgr_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	case CMD_vimgrep:     return (char_u *)"vimgrep";
	case CMD_lvimgrep:    return (char_u *)"lvimgrep";
	case CMD_vimgrepadd:  return (char_u *)"vimgrepadd";
	case CMD_lvimgrepadd: return (char_u *)"lvimgrepadd";
	case CMD_grep:	      return (char_u *)"grep";
	case CMD_lgrep:	      return (char_u *)"lgrep";
	case CMD_grepadd:     return (char_u *)"grepadd";
	case CMD_lgrepadd:    return (char_u *)"lgrepadd";
	default: return NULL;
    }
}