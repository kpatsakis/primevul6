Bool gf_opus_parse_header(GF_OpusParser *opus, u8 *data, u32 data_len)
{
	char tag[9];
	GF_BitStream *bs = gf_bs_new(data, data_len, GF_BITSTREAM_READ);
	gf_bs_read_data(bs, tag, 8);
	tag[8]=0;

	if (memcmp(data, "OpusHead", sizeof(char)*8)) {
		gf_bs_del(bs);
		return GF_FALSE;
	}
	/*Identification Header*/
	opus->version = gf_bs_read_u8(bs); /*version*/
	if (opus->version != 1) {
		gf_bs_del(bs);
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[Opus] Unsupported version %d\n", opus->version));
		return GF_FALSE;
	}
	opus->OutputChannelCount = gf_bs_read_u8(bs);
	opus->PreSkip = gf_bs_read_u16_le(bs);
	opus->InputSampleRate = gf_bs_read_u32_le(bs);
	opus->OutputGain = gf_bs_read_u16_le(bs);
	opus->ChannelMappingFamily = gf_bs_read_u8(bs);
	if (opus->ChannelMappingFamily != 0) {
		opus->StreamCount = gf_bs_read_u8(bs);
		opus->CoupledCount = gf_bs_read_u8(bs);
		gf_bs_read_data(bs, (char *) opus->ChannelMapping, opus->OutputChannelCount);
	}
	gf_bs_del(bs);
	return GF_TRUE;
}