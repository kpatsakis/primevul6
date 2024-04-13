const char *gf_m2ts_get_stream_name(u32 streamType)
{
	switch (streamType) {
	case GF_M2TS_VIDEO_MPEG1:
		return "MPEG-1 Video";
	case GF_M2TS_VIDEO_MPEG2:
		return "MPEG-2 Video";
	case GF_M2TS_AUDIO_MPEG1:
		return "MPEG-1 Audio";
	case GF_M2TS_AUDIO_MPEG2:
		return "MPEG-2 Audio";
	case GF_M2TS_PRIVATE_SECTION:
		return "Private Section";
	case GF_M2TS_PRIVATE_DATA:
		return "Private Data";
	case GF_M2TS_AUDIO_AAC:
		return "AAC Audio";
	case GF_M2TS_VIDEO_MPEG4:
		return "MPEG-4 Video";
	case GF_M2TS_VIDEO_H264:
		return "MPEG-4/H264 Video";
	case GF_M2TS_VIDEO_SVC:
		return "H264-SVC Video";
	case GF_M2TS_VIDEO_HEVC:
		return "HEVC Video";
	case GF_M2TS_VIDEO_SHVC:
		return "SHVC Video";
	case GF_M2TS_VIDEO_SHVC_TEMPORAL:
		return "SHVC Video Temporal Sublayer";
	case GF_M2TS_VIDEO_MHVC:
		return "MHVC Video";
	case GF_M2TS_VIDEO_MHVC_TEMPORAL:
		return "MHVC Video Temporal Sublayer";

	case GF_M2TS_AUDIO_AC3:
		return "Dolby AC3 Audio";
	case GF_M2TS_AUDIO_DTS:
		return "Dolby DTS Audio";
	case GF_M2TS_SUBTITLE_DVB:
		return "DVB Subtitle";
	case GF_M2TS_SYSTEMS_MPEG4_PES:
		return "MPEG-4 SL (PES)";
	case GF_M2TS_SYSTEMS_MPEG4_SECTIONS:
		return "MPEG-4 SL (Section)";
	case GF_M2TS_MPE_SECTIONS:
		return "MPE (Section)";

	case GF_M2TS_METADATA_PES:
		return "Metadata (PES)";
	case GF_M2TS_METADATA_ID3_HLS:
		return "ID3/HLS Metadata (PES)";

	default:
		return "Unknown";
	}
}