PJ_DEF(pj_status_t) pjmedia_sdp_media_add_attr( pjmedia_sdp_media *m,
						pjmedia_sdp_attr *attr)
{
    return pjmedia_sdp_attr_add(&m->attr_count, m->attr, attr);
}