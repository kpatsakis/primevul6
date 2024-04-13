PJ_DEF(unsigned) pjmedia_sdp_attr_remove_all(unsigned *count,
					     pjmedia_sdp_attr *attr_array[],
					     const char *name)
{
    unsigned i, removed = 0;
    pj_str_t attr_name;

    PJ_ASSERT_RETURN(count && attr_array && name, PJ_EINVAL);
    PJ_ASSERT_RETURN(*count <= PJMEDIA_MAX_SDP_ATTR, PJ_ETOOMANY);

    attr_name.ptr = (char*)name;
    attr_name.slen = pj_ansi_strlen(name);

    for (i=0; i<*count; ) {
	if (pj_strcmp(&attr_array[i]->name, &attr_name)==0) {
	    pj_array_erase(attr_array, sizeof(pjmedia_sdp_attr*),
			   *count, i);
	    --(*count);
	    ++removed;
	} else {
	    ++i;
	}   
    }

    return removed;
}