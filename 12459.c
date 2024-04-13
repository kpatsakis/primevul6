PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_clone(pj_pool_t *pool, 
						 const pjmedia_sdp_attr *rhs)
{
    pjmedia_sdp_attr *attr;
    
    PJ_ASSERT_RETURN(pool && rhs, NULL);

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);

    pj_strdup(pool, &attr->name, &rhs->name);
    pj_strdup_with_null(pool, &attr->value, &rhs->value);

    return attr;
}