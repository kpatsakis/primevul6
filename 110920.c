const char *gf_hevc_get_profile_name(u8 video_prof)
{
	switch (video_prof) {
	case 0x01:
		return "Main";
	case 0x02:
		return "Main 10";
	case 0x03:
		return "Main Still Picture";
	default:
		return "Unknown";
	}
}