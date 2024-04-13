PJ_DEF(pjmedia_sdp_media*) pjmedia_sdp_media_clone(
						 pj_pool_t *pool, 
						 const pjmedia_sdp_media *rhs)
{
    unsigned int i;
    pjmedia_sdp_media *m = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_media);
    PJ_ASSERT_RETURN(m != NULL, NULL);

    pj_strdup (pool, &m->desc.media, &rhs->desc.media);
    m->desc.port = rhs->desc.port;
    m->desc.port_count = rhs->desc.port_count;
    pj_strdup (pool, &m->desc.transport, &rhs->desc.transport);
    m->desc.fmt_count = rhs->desc.fmt_count;
    for (i=0; i<rhs->desc.fmt_count; ++i)
	pj_strdup(pool, &m->desc.fmt[i], &rhs->desc.fmt[i]);

    if (rhs->conn) {
	m->conn = pjmedia_sdp_conn_clone (pool, rhs->conn);
	PJ_ASSERT_RETURN(m->conn != NULL, NULL);
    } else {
	m->conn = NULL;
    }

    m->bandw_count = rhs->bandw_count;
    for (i=0; i < rhs->bandw_count; ++i) {
	m->bandw[i] = pjmedia_sdp_bandw_clone (pool, rhs->bandw[i]);
	PJ_ASSERT_RETURN(m->bandw[i] != NULL, NULL);
    }

    m->attr_count = rhs->attr_count;
    for (i=0; i < rhs->attr_count; ++i) {
	m->attr[i] = pjmedia_sdp_attr_clone (pool, rhs->attr[i]);
	PJ_ASSERT_RETURN(m->attr[i] != NULL, NULL);
    }

    return m;
}