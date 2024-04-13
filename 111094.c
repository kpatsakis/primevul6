void gf_hevc_parse_sei(char *buffer, u32 nal_size, HEVCState *hevc)
{
	gf_hevc_vvc_parse_sei(buffer, nal_size, hevc, NULL);
}