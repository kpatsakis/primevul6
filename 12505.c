PJ_DEF(pj_status_t) pjmedia_sdp_session_add_attr(pjmedia_sdp_session *s,
						 pjmedia_sdp_attr *attr)
{
    return pjmedia_sdp_attr_add(&s->attr_count, s->attr, attr);
}