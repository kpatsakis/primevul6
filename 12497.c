PJ_DEF(pj_status_t) pjmedia_sdp_attr_remove( unsigned *count,
					     pjmedia_sdp_attr *attr_array[],
					     pjmedia_sdp_attr *attr )
{
    unsigned i, removed=0;

    PJ_ASSERT_RETURN(count && attr_array && attr, PJ_EINVAL);
    PJ_ASSERT_RETURN(*count <= PJMEDIA_MAX_SDP_ATTR, PJ_ETOOMANY);

    for (i=0; i<*count; ) {
	if (attr_array[i] == attr) {
	    pj_array_erase(attr_array, sizeof(pjmedia_sdp_attr*),
			   *count, i);
	    --(*count);
	    ++removed;
	} else {
	    ++i;
	}
    }

    return removed ? PJ_SUCCESS : PJ_ENOTFOUND;
}