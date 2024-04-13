PJ_DEF(int) pjmedia_sdp_print( const pjmedia_sdp_session *desc, 
			       char *buf, pj_size_t size)
{
    return print_session(desc, buf, size);
}