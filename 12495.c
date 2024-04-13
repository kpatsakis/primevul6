PJ_DEF(pj_status_t) pjmedia_sdp_attr_get_fmtp( const pjmedia_sdp_attr *attr,
					       pjmedia_sdp_fmtp *fmtp)
{
    const char *p = attr->value.ptr;
    const char *end = attr->value.ptr + attr->value.slen;
    pj_str_t token;

    PJ_ASSERT_RETURN(pj_strcmp2(&attr->name, "fmtp")==0, PJ_EINVALIDOP);

    if (attr->value.slen == 0)
        return PJMEDIA_SDP_EINATTR;

    /* fmtp BNF:
     *	a=fmtp:<format> <format specific parameter>
     */

    /* Get format. */
    token.ptr = (char*)p;
    while (pj_isdigit(*p) && p!=end)
	++p;
    token.slen = p - token.ptr;
    if (token.slen == 0)
	return PJMEDIA_SDP_EINFMTP;

    fmtp->fmt = token;

    /* Expecting space after format. */
    if (*p != ' ') return PJMEDIA_SDP_EINFMTP;

    /* Get space. */
    ++p;

    /* Set the remaining string as fmtp format parameter. */
    fmtp->fmt_param.ptr = (char*)p;
    fmtp->fmt_param.slen = end - p;

    return PJ_SUCCESS;
}