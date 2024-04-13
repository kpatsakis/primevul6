PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_create_rtcp(pj_pool_t *pool,
						       const pj_sockaddr *a)
{
    enum {
	ATTR_LEN = PJ_INET6_ADDRSTRLEN+16
    };
    char tmp_addr[PJ_INET6_ADDRSTRLEN];
    pjmedia_sdp_attr *attr;

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);
    attr->name = pj_str("rtcp");
    attr->value.ptr = (char*) pj_pool_alloc(pool, ATTR_LEN);
    if (a->addr.sa_family == pj_AF_INET()) {
	attr->value.slen = 
	    pj_ansi_snprintf(attr->value.ptr, ATTR_LEN,
			    "%u IN IP4 %s",
			    pj_sockaddr_get_port(a),
			    pj_sockaddr_print(a, tmp_addr, 
					      sizeof(tmp_addr), 0));
    } else if (a->addr.sa_family == pj_AF_INET6()) {
	attr->value.slen = 
	    pj_ansi_snprintf(attr->value.ptr, ATTR_LEN,
			    "%u IN IP6 %s",
			    pj_sockaddr_get_port(a),
			    pj_sockaddr_print(a, tmp_addr, 
					      sizeof(tmp_addr), 0));

    } else {
	pj_assert(!"Unsupported address family");
	return NULL;
    }

    return attr;
}