PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_create( pj_pool_t *pool,
						   const char *name,
						   const pj_str_t *value)
{
    pjmedia_sdp_attr *attr;

    PJ_ASSERT_RETURN(pool && name, NULL);

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);
    pj_strdup2(pool, &attr->name, name);

    if (value)
	pj_strdup_with_null(pool, &attr->value, value);
    else {
	attr->value.ptr = NULL;
	attr->value.slen = 0;
    }

    return attr;
}