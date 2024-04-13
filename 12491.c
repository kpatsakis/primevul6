PJ_DEF(pjmedia_sdp_conn*) pjmedia_sdp_conn_clone (pj_pool_t *pool, 
						  const pjmedia_sdp_conn *rhs)
{
    pjmedia_sdp_conn *c = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_conn);
    if (!c) return NULL;

    if (!pj_strdup (pool, &c->net_type, &rhs->net_type)) return NULL;
    if (!pj_strdup (pool, &c->addr_type, &rhs->addr_type)) return NULL;
    if (!pj_strdup (pool, &c->addr, &rhs->addr)) return NULL;

    return c;
}