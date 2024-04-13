GF_Err gf_media_parse_ivf_file_header(GF_BitStream *bs, u32 *width, u32 *height, u32 *codec_fourcc, u32 *timebase_num, u32 *timebase_den, u32 *num_frames)
{
	u32 dw = 0;

	if (!width || !height || !codec_fourcc || !timebase_den || !timebase_num || !num_frames) {
		assert(0);
		return GF_BAD_PARAM;
	}

	if (gf_bs_available(bs) < IVF_FILE_HEADER_SIZE) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[IVF] Not enough bytes available ("LLU").\n", gf_bs_available(bs)));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	dw = gf_bs_read_u32(bs);
	if (dw != GF_4CC('D', 'K', 'I', 'F')) {
		GF_LOG(GF_LOG_INFO, GF_LOG_CODING, ("[IVF] Invalid signature\n"));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	dw = gf_bs_read_u16_le(bs);
	if (dw != 0) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[IVF] Wrong IVF version. 0 expected, got %u\n", dw));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	dw = gf_bs_read_u16_le(bs); //length of header in bytes
	if (dw != IVF_FILE_HEADER_SIZE) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[IVF] Wrong IVF header length. Expected 32 bytes, got %u\n", dw));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	*codec_fourcc = gf_bs_read_u32(bs);

	*width = gf_bs_read_u16_le(bs);
	*height = gf_bs_read_u16_le(bs);

	*timebase_num = gf_bs_read_u32_le(bs);
	*timebase_den = gf_bs_read_u32_le(bs);

	*num_frames = gf_bs_read_u32_le(bs);
	gf_bs_read_u32_le(bs); //skip unused

	return GF_OK;
}