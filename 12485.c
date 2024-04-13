PJ_DEF(pj_status_t) pjmedia_sdp_attr_add(unsigned *count,
					 pjmedia_sdp_attr *attr_array[],
					 pjmedia_sdp_attr *attr)
{
    PJ_ASSERT_RETURN(count && attr_array && attr, PJ_EINVAL);
    PJ_ASSERT_RETURN(*count < PJMEDIA_MAX_SDP_ATTR, PJ_ETOOMANY);

    attr_array[*count] = attr;
    (*count)++;

    return PJ_SUCCESS;
}