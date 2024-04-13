PJ_DEF(pjmedia_sdp_session*) pjmedia_sdp_session_clone( pj_pool_t *pool,
			   			const pjmedia_sdp_session *rhs)
{
    pjmedia_sdp_session *sess;
    unsigned i;

    PJ_ASSERT_RETURN(pool && rhs, NULL);

    sess = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_session);
    PJ_ASSERT_RETURN(sess != NULL, NULL);

    /* Clone origin line. */
    pj_strdup(pool, &sess->origin.user, &rhs->origin.user);
    sess->origin.id = rhs->origin.id;
    sess->origin.version = rhs->origin.version;
    pj_strdup(pool, &sess->origin.net_type, &rhs->origin.net_type);
    pj_strdup(pool, &sess->origin.addr_type, &rhs->origin.addr_type);
    pj_strdup(pool, &sess->origin.addr, &rhs->origin.addr);

    /* Clone subject line. */
    pj_strdup(pool, &sess->name, &rhs->name);

    /* Clone connection line */
    if (rhs->conn) {
	sess->conn = pjmedia_sdp_conn_clone(pool, rhs->conn);
	PJ_ASSERT_RETURN(sess->conn != NULL, NULL);
    }

    /* Duplicate bandwidth info */
    sess->bandw_count = rhs->bandw_count;
    for (i=0; i<rhs->bandw_count; ++i) {
	sess->bandw[i] = pjmedia_sdp_bandw_clone(pool, rhs->bandw[i]);
    }

    /* Clone time line. */
    sess->time.start = rhs->time.start;
    sess->time.stop = rhs->time.stop;

    /* Duplicate session attributes. */
    sess->attr_count = rhs->attr_count;
    for (i=0; i<rhs->attr_count; ++i) {
	sess->attr[i] = pjmedia_sdp_attr_clone(pool, rhs->attr[i]);
    }

    /* Duplicate media descriptors. */
    sess->media_count = rhs->media_count;
    for (i=0; i<rhs->media_count; ++i) {
	sess->media[i] = pjmedia_sdp_media_clone(pool, rhs->media[i]);
    }

    return sess;
}