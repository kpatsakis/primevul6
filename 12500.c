static pj_ssize_t print_bandw(const pjmedia_sdp_bandw *bandw,
			      char *buf, pj_size_t len)
{
    char *p = buf;

    if ((int)len < bandw->modifier.slen + 10 + 5)
	return -1;

    *p++ = 'b';
    *p++ = '=';
    pj_memcpy(p, bandw->modifier.ptr, bandw->modifier.slen);
    p += bandw->modifier.slen;
    *p++ = ':';
    p += pj_utoa(bandw->value, p);

    *p++ = '\r';
    *p++ = '\n';
    return p-buf;
}