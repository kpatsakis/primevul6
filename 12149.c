cbuffer_get_auname(cmdidx_T cmdidx)
{
    switch (cmdidx)
    {
	case CMD_cbuffer:	return (char_u *)"cbuffer";
	case CMD_cgetbuffer:	return (char_u *)"cgetbuffer";
	case CMD_caddbuffer:	return (char_u *)"caddbuffer";
	case CMD_lbuffer:	return (char_u *)"lbuffer";
	case CMD_lgetbuffer:	return (char_u *)"lgetbuffer";
	case CMD_laddbuffer:	return (char_u *)"laddbuffer";
	default:		return NULL;
    }
}