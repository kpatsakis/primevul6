PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_find (unsigned count, 
				       pjmedia_sdp_attr *const attr_array[],
				       const pj_str_t *name,
				       const pj_str_t *c_fmt)
{
    unsigned i;
    unsigned c_pt = 0xFFFF;

    PJ_ASSERT_RETURN(count <= PJMEDIA_MAX_SDP_ATTR, NULL);

    if (c_fmt)
	c_pt = pj_strtoul(c_fmt);

    for (i=0; i<count; ++i) {
	if (pj_strcmp(&attr_array[i]->name, name) == 0) {
	    const pjmedia_sdp_attr *a = attr_array[i];
	    if (c_fmt) {
		unsigned pt = (unsigned) pj_strtoul2(&a->value, NULL, 10);
		if (pt == c_pt) {
		    return (pjmedia_sdp_attr*)a;
		}
	    } else 
		return (pjmedia_sdp_attr*)a;
	}
    }
    return NULL;
}