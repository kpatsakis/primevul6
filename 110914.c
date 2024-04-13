s32 gf_hevc_read_vps_ex(u8 *data, u32 *size, HEVCState *hevc, Bool remove_extensions)
{
	GF_BitStream *bs;
	char *data_without_emulation_bytes = NULL;
	u32 data_without_emulation_bytes_size = 0;
	s32 vps_id = -1;

	/*still contains emulation bytes*/
	data_without_emulation_bytes_size = remove_extensions ? gf_media_nalu_emulation_bytes_remove_count(data, (*size)) : 0;
	if (!data_without_emulation_bytes_size) {
		bs = gf_bs_new(data, (*size), GF_BITSTREAM_READ);
		gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	}
	//when removing VPS ext, we have to get the full buffer without emulation prevention bytes becuase we do a bit-by-bit copy of the vps
	else {
		data_without_emulation_bytes = gf_malloc((*size) * sizeof(char));
		data_without_emulation_bytes_size = gf_media_nalu_remove_emulation_bytes(data, data_without_emulation_bytes, (*size));
		bs = gf_bs_new(data_without_emulation_bytes, data_without_emulation_bytes_size, GF_BITSTREAM_READ);
	}
	if (!bs) goto exit;


	if (!hevc_parse_nal_header(bs, NULL, NULL, NULL)) goto exit;

	vps_id = gf_hevc_read_vps_bs_internal(bs, hevc, remove_extensions);
	if (vps_id < 0) goto exit;

	if (remove_extensions) {
		u8 *new_vps;
		u32 new_vps_size, emulation_bytes;
		u32 bit_pos = gf_bs_get_bit_offset(bs);
		GF_BitStream *w_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_bs_seek(bs, 0);
		gf_bs_write_u8(w_bs, gf_bs_read_u8(bs) );
		gf_bs_write_u8(w_bs, gf_bs_read_u8(bs) );
		gf_bs_write_u8(w_bs, gf_bs_read_u8(bs) );
		gf_bs_write_u8(w_bs, gf_bs_read_u8(bs) );
		gf_bs_write_u16(w_bs, gf_bs_read_u16(bs) );
		bit_pos -= 48;
		while (bit_pos) {
			u32 v = gf_bs_read_int(bs, 1);
			gf_bs_write_int(w_bs, v, 1);
			bit_pos--;
		}
		/*vps extension flag*/
		gf_bs_write_int(w_bs, 0, 1);
		new_vps = NULL;
		gf_bs_get_content(w_bs, &new_vps, &new_vps_size);
		gf_bs_del(w_bs);

		emulation_bytes = gf_media_nalu_emulation_bytes_add_count(new_vps, new_vps_size);
		if (emulation_bytes + new_vps_size > *size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("Buffer too small to rewrite VPS - skipping rewrite\n"));
		}
		else {
			*size = gf_media_nalu_add_emulation_bytes(new_vps, data, new_vps_size);
		}
		if (new_vps)
			gf_free(new_vps);
	}

exit:
	if (bs)
		gf_bs_del(bs);
	if (data_without_emulation_bytes) gf_free(data_without_emulation_bytes);
	return vps_id;
}