static pj_status_t validate_sdp_conn(const pjmedia_sdp_conn *c)
{
    CHECK( c, PJ_EINVAL);
    CHECK( pj_strcmp2(&c->net_type, "IN")==0, PJMEDIA_SDP_EINCONN);
    CHECK( pj_strcmp2(&c->addr_type, "IP4")==0 ||
	   pj_strcmp2(&c->addr_type, "IP6")==0, 
	   PJMEDIA_SDP_EINCONN);
    CHECK( c->addr.slen != 0, PJMEDIA_SDP_EINCONN);

    return PJ_SUCCESS;
}