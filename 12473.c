PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_create_ssrc( pj_pool_t *pool,
							pj_uint32_t ssrc,
							const pj_str_t *cname)
{
    pjmedia_sdp_attr *attr;

    if (cname->slen == 0)
        return NULL;

    attr = PJ_POOL_ALLOC_T(pool, pjmedia_sdp_attr);
    attr->name = pj_str("ssrc");
    attr->value.ptr = (char*) pj_pool_alloc(pool, cname->slen+7 /* " cname:"*/
    						  + 10 /* 32-bit integer */
    						  + 1 /* NULL */);
    attr->value.slen = pj_ansi_snprintf(attr->value.ptr, cname->slen+18,
    					"%u cname:%.*s", ssrc,
			   	   	(int)cname->slen, cname->ptr);

    return attr;
}