PJ_DEF(int) pjmedia_sdp_media_print(const pjmedia_sdp_media *media,
			       char *buf, pj_size_t size)
{
	return print_media_desc(media, buf, size);
}