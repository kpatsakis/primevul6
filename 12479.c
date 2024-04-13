PJ_DEF(pjmedia_sdp_attr*) pjmedia_sdp_attr_find2(unsigned count, 
				       pjmedia_sdp_attr *const attr_array[],
				       const char *c_name,
				       const pj_str_t *c_fmt)
{
    pj_str_t name;

    name.ptr = (char*)c_name;
    name.slen = pj_ansi_strlen(c_name);

    return pjmedia_sdp_attr_find(count, attr_array, &name, c_fmt);
}