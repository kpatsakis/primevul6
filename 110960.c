Bool gf_ac3_parser_bs(GF_BitStream *bs, GF_AC3Config *hdr, Bool full_parse)
{
	u32 fscod, frmsizecod, bsid, ac3_mod, freq, framesize, bsmod, syncword;
	u64 pos;
	if (!hdr || (gf_bs_available(bs) < 6)) return GF_FALSE;
	if (!AC3_FindSyncCodeBS(bs)) return GF_FALSE;

	pos = gf_bs_get_position(bs);

	syncword = gf_bs_read_u16(bs);
	if (syncword != 0x0B77) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AC3] Wrong sync word detected (0x%X - expecting 0x0B77).\n", syncword));
		return GF_FALSE;
	}
	gf_bs_read_int_log(bs, 16, "crc1");
	fscod = gf_bs_read_int_log(bs, 2, "fscod");
	frmsizecod = gf_bs_read_int_log(bs, 6, "frmsizecod");
	bsid = gf_bs_read_int_log(bs, 5, "bsid");
	bsmod = gf_bs_read_int_log(bs, 3, "bsmod");
	ac3_mod = gf_bs_read_int_log(bs, 3, "ac3_mod");
	if (frmsizecod >= 2 * sizeof(ac3_sizecod_to_bitrate) / sizeof(u32))
		return GF_FALSE;

	hdr->bitrate = ac3_sizecod_to_bitrate[frmsizecod / 2];
	if (bsid > 8) hdr->bitrate = hdr->bitrate >> (bsid - 8);

	switch (fscod) {
	case 0:
		if (frmsizecod >=  2 * sizeof(ac3_sizecod0_to_framesize) / sizeof(u32))
			return GF_FALSE;
		freq = 48000;
		framesize = ac3_sizecod0_to_framesize[frmsizecod / 2] * 2;
		break;
	case 1:
		if (frmsizecod >= 2 * sizeof(ac3_sizecod1_to_framesize) / sizeof(u32))
			return GF_FALSE;
		freq = 44100;
		framesize = (ac3_sizecod1_to_framesize[frmsizecod / 2] + (frmsizecod & 0x1)) * 2;
		break;
	case 2:
		if (frmsizecod >= 2 * sizeof(ac3_sizecod2_to_framesize) / sizeof(u32))
			return GF_FALSE;
		freq = 32000;
		framesize = ac3_sizecod2_to_framesize[frmsizecod / 2] * 2;
		break;
	default:
		return GF_FALSE;
	}
	hdr->sample_rate = freq;
	hdr->framesize = framesize;

	if (full_parse) {
		hdr->streams[0].bsid = bsid;
		hdr->streams[0].bsmod = bsmod;
		hdr->streams[0].acmod = ac3_mod;
		hdr->streams[0].lfon = 0;
		hdr->streams[0].fscod = fscod;
		hdr->brcode = frmsizecod / 2;
	}
	if (ac3_mod >= 2 * sizeof(ac3_mod_to_chans) / sizeof(u32))
		return GF_FALSE;

	hdr->channels = ac3_mod_to_chans[ac3_mod];
	if ((ac3_mod & 0x1) && (ac3_mod != 1)) gf_bs_read_int_log(bs, 2, "cmixlev");
	if (ac3_mod & 0x4) gf_bs_read_int_log(bs, 2, "surmixlev");
	if (ac3_mod == 0x2) gf_bs_read_int_log(bs, 2, "dsurmod");

	if (gf_bs_read_int_log(bs, 1, "lfeon")) {
		hdr->channels += 1;
		hdr->streams[0].lfon = 1;
	}

	gf_bs_seek(bs, pos);

	return GF_TRUE;
}