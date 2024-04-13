u8 gf_mp3_object_type_indication(u32 hdr)
{
	switch (gf_mp3_version(hdr)) {
	case 3:
		return GF_CODECID_MPEG_AUDIO;
	case 2:
	case 0:
		return GF_CODECID_MPEG2_PART3;
	default:
		return 0x00;
	}
}