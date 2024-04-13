s32 gf_avc_read_sps(const u8 *sps_data, u32 sps_size, AVCState *avc, u32 subseq_sps, u32 *vui_flag_pos)
{
	s32 sps_id = -1;
	GF_BitStream *bs;
	char *sps_data_without_emulation_bytes = NULL;
	u32 sps_data_without_emulation_bytes_size = 0;

	if (vui_flag_pos) {
		/*SPS still contains emulation bytes*/
		sps_data_without_emulation_bytes = gf_malloc(sps_size * sizeof(char));
		sps_data_without_emulation_bytes_size = gf_media_nalu_remove_emulation_bytes(sps_data, sps_data_without_emulation_bytes, sps_size);
		bs = gf_bs_new(sps_data_without_emulation_bytes, sps_data_without_emulation_bytes_size, GF_BITSTREAM_READ);

		*vui_flag_pos = 0;
	}
	else {
		bs = gf_bs_new(sps_data, sps_size, GF_BITSTREAM_READ);
	}

	if (!bs) {
		sps_id = -1;
		goto exit;
	}

	sps_id = gf_avc_read_sps_bs(bs, avc, subseq_sps, vui_flag_pos);

exit:
	gf_bs_del(bs);
	if (sps_data_without_emulation_bytes) gf_free(sps_data_without_emulation_bytes);
	return sps_id;
}