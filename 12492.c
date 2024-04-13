PJ_DEF(pj_status_t) pjmedia_sdp_attr_to_rtpmap(pj_pool_t *pool,
					       const pjmedia_sdp_attr *attr,
					       pjmedia_sdp_rtpmap **p_rtpmap)
{
    PJ_ASSERT_RETURN(pool && attr && p_rtpmap, PJ_EINVAL);

    *p_rtpmap = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_rtpmap);
    PJ_ASSERT_RETURN(*p_rtpmap, PJ_ENOMEM);

    return pjmedia_sdp_attr_get_rtpmap(attr, *p_rtpmap);
}