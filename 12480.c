PJ_DEF(unsigned) pjmedia_sdp_media_remove_all_attr(pjmedia_sdp_media *m,
						   const char *name)
{
    return pjmedia_sdp_attr_remove_all(&m->attr_count, m->attr, name);
}