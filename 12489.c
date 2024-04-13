pjmedia_sdp_bandw_clone (pj_pool_t *pool, 
			 const pjmedia_sdp_bandw *rhs)
{
    pjmedia_sdp_bandw *b = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_bandw);
    if (!b) return NULL;

    if (!pj_strdup (pool, &b->modifier, &rhs->modifier)) return NULL;
    b->value = rhs->value;

    return b;
}