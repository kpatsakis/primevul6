u16 gf_mp3_sampling_rate(u32 hdr)
{
	u16 res;
	/* extract the necessary fields from the MP3 header */
	u8 version = gf_mp3_version(hdr);
	u8 sampleRateIndex = (hdr >> 10) & 0x3;

	switch (sampleRateIndex) {
	case 0:
		res = 44100;
		break;
	case 1:
		res = 48000;
		break;
	case 2:
		res = 32000;
		break;
	default:
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[MPEG-1/2 Audio] Samplerate index not valid\n"));
		return 0;
	}
	/*reserved or MPEG-1*/
	if (version & 1) return res;

	/*MPEG-2*/
	res /= 2;
	/*MPEG-2.5*/
	if (version == 0) res /= 2;
	return res;
}