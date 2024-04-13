static int print_connection_info( pjmedia_sdp_conn *c, char *buf, int len)
{
    int printed;

    printed = pj_ansi_snprintf(buf, len, "c=%.*s %.*s %.*s\r\n",
			       (int)c->net_type.slen,
			       c->net_type.ptr,
			       (int)c->addr_type.slen,
			       c->addr_type.ptr,
			       (int)c->addr.slen,
			       c->addr.ptr);
    if (printed < 1 || printed >= len)
	return -1;

    return printed;
}