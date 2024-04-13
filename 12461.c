static int print_session(const pjmedia_sdp_session *ses, 
			 char *buf, pj_ssize_t len)
{
    char *p = buf;
    char *end = buf+len;
    unsigned i;
    int printed;

    /* Check length for v= and o= lines. */
    if (len < 5+ 
	      2+ses->origin.user.slen+18+
	      ses->origin.net_type.slen+ses->origin.addr.slen + 2)
    {
	return -1;
    }

    /* SDP version (v= line) */
    pj_memcpy(p, "v=0\r\n", 5);
    p += 5;

    /* Owner (o=) line. */
    *p++ = 'o';
    *p++ = '=';
    pj_memcpy(p, ses->origin.user.ptr, ses->origin.user.slen);
    p += ses->origin.user.slen;
    *p++ = ' ';
    printed = pj_utoa(ses->origin.id, p);
    p += printed;
    *p++ = ' ';
    printed = pj_utoa(ses->origin.version, p);
    p += printed;
    *p++ = ' ';
    pj_memcpy(p, ses->origin.net_type.ptr, ses->origin.net_type.slen);
    p += ses->origin.net_type.slen;
    *p++ = ' ';
    pj_memcpy(p, ses->origin.addr_type.ptr, ses->origin.addr_type.slen);
    p += ses->origin.addr_type.slen;
    *p++ = ' ';
    pj_memcpy(p, ses->origin.addr.ptr, ses->origin.addr.slen);
    p += ses->origin.addr.slen;
    *p++ = '\r';
    *p++ = '\n';

    /* Session name (s=) line. */
    if ((end-p)  < 8+ses->name.slen) {
	return -1;
    }
    *p++ = 's';
    *p++ = '=';
    pj_memcpy(p, ses->name.ptr, ses->name.slen);
    p += ses->name.slen;
    *p++ = '\r';
    *p++ = '\n';

    /* Connection line (c=) if exist. */
    if (ses->conn) {
	printed = print_connection_info(ses->conn, p, (int)(end-p));
	if (printed < 1) {
	    return -1;
	}
	p += printed;
    }

    /* print optional bandwidth info. */
    for (i=0; i<ses->bandw_count; ++i) {
	printed = (int)print_bandw(ses->bandw[i], p, end-p);
	if (printed < 1) {
	    return -1;
	}
	p += printed;
    }

    /* Time */
    if ((end-p) < 24) {
	return -1;
    }
    *p++ = 't';
    *p++ = '=';
    printed = pj_utoa(ses->time.start, p);
    p += printed;
    *p++ = ' ';
    printed = pj_utoa(ses->time.stop, p);
    p += printed;
    *p++ = '\r';
    *p++ = '\n';

    /* Print all attribute (a=) lines. */
    for (i=0; i<ses->attr_count; ++i) {
	printed = (int)print_attr(ses->attr[i], p, end-p);
	if (printed < 0) {
	    return -1;
	}
	p += printed;
    }

    /* Print media (m=) lines. */
    for (i=0; i<ses->media_count; ++i) {
	printed = print_media_desc(ses->media[i], p, (int)(end-p));
	if (printed < 0) {
	    return -1;
	}
	p += printed;
    }

    return (int)(p-buf);
}