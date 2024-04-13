PJ_DEF(pjmedia_sdp_media*) pjmedia_sdp_media_clone_deactivate(
						pj_pool_t *pool,
						const pjmedia_sdp_media *rhs)
{
    unsigned int i;
    pjmedia_sdp_media *m;

    PJ_ASSERT_RETURN(pool && rhs, NULL);

    m = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_media);
    pj_memcpy(m, rhs, sizeof(*m));

    /* Clone the media line only */
    pj_strdup (pool, &m->desc.media, &rhs->desc.media);
    pj_strdup (pool, &m->desc.transport, &rhs->desc.transport);
    for (i=0; i<rhs->desc.fmt_count; ++i)
	pj_strdup(pool, &m->desc.fmt[i], &rhs->desc.fmt[i]);

    if (rhs->conn) {
	m->conn = pjmedia_sdp_conn_clone (pool, rhs->conn);
	PJ_ASSERT_RETURN(m->conn != NULL, NULL);
    }

    m->bandw_count = rhs->bandw_count;
    for (i=0; i < rhs->bandw_count; ++i) {
	m->bandw[i] = pjmedia_sdp_bandw_clone (pool, rhs->bandw[i]);
	PJ_ASSERT_RETURN(m->bandw[i] != NULL, NULL);
    }

    /* And deactivate it */
    pjmedia_sdp_media_deactivate(pool, m);

    return m;
}