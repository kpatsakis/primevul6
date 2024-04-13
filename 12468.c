static pj_ssize_t print_attr(const pjmedia_sdp_attr *attr, 
			     char *buf, pj_size_t len)
{
    char *p = buf;

    if ((int)len < attr->name.slen + attr->value.slen + 10)
	return -1;

    *p++ = 'a';
    *p++ = '=';
    pj_memcpy(p, attr->name.ptr, attr->name.slen);
    p += attr->name.slen;
    

    if (attr->value.slen) {
	*p++ = ':';
	pj_memcpy(p, attr->value.ptr, attr->value.slen);
	p += attr->value.slen;
    }

    *p++ = '\r';
    *p++ = '\n';
    return p-buf;
}