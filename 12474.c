PJ_DEF(pj_status_t) pjmedia_sdp_validate(const pjmedia_sdp_session *sdp)
{
    return pjmedia_sdp_validate2(sdp, PJ_TRUE);
}