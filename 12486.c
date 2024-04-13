PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_media_find_attr(
				const pjmedia_sdp_media *m,
				const pj_str_t *name, const pj_str_t *fmt)
{
    PJ_ASSERT_RETURN(m && name, NULL);
    return pjmedia_sdp_attr_find(m->attr_count, m->attr, name, fmt);
}