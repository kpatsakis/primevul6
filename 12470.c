PJ_DEF(pj_status_t) pjmedia_sdp_rtpmap_to_attr(pj_pool_t *pool,
					       const pjmedia_sdp_rtpmap *rtpmap,
					       pjmedia_sdp_attr **p_attr)
{
    pjmedia_sdp_attr *attr;
    char tempbuf[128];
    int len;

    /* Check arguments. */
    PJ_ASSERT_RETURN(pool && rtpmap && p_attr, PJ_EINVAL);

    /* Check that mandatory attributes are specified. */
    PJ_ASSERT_RETURN(rtpmap->enc_name.slen && rtpmap->clock_rate,
		     PJMEDIA_SDP_EINRTPMAP);


    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);
    PJ_ASSERT_RETURN(attr != NULL, PJ_ENOMEM);

    attr->name.ptr = "rtpmap";
    attr->name.slen = 6;

    /* Format: ":pt enc_name/clock_rate[/param]" */
    len = pj_ansi_snprintf(tempbuf, sizeof(tempbuf), 
			   "%.*s %.*s/%u%s%.*s",
			   (int)rtpmap->pt.slen,
			   rtpmap->pt.ptr,
			   (int)rtpmap->enc_name.slen,
			   rtpmap->enc_name.ptr,
			   rtpmap->clock_rate,
			   (rtpmap->param.slen ? "/" : ""),
			   (int)rtpmap->param.slen,
			   rtpmap->param.ptr);

    if (len < 1 || len >= (int)sizeof(tempbuf))
	return PJMEDIA_SDP_ERTPMAPTOOLONG;

    attr->value.slen = len;
    attr->value.ptr = (char*) pj_pool_alloc(pool, attr->value.slen+1);
    pj_memcpy(attr->value.ptr, tempbuf, attr->value.slen+1);

    *p_attr = attr;
    return PJ_SUCCESS;
}