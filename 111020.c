GF_Err gf_media_prores_parse_bs(GF_BitStream *bs, GF_ProResFrameInfo *prores_frame)
{
	u32 i, j;
	u64 start, pos;
	memset(prores_frame, 0, sizeof(GF_ProResFrameInfo));

	start = gf_bs_get_position(bs);
	if (gf_bs_available(bs) < 10)
		return GF_BUFFER_TOO_SMALL;

	prores_frame->frame_size = gf_bs_read_u32(bs);
	prores_frame->frame_identifier = gf_bs_read_u32(bs);
	if (prores_frame->frame_identifier != GF_4CC('i','c','p','f')) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[ProRes] Invalid frame identifier, expected \"icpf\" got \"%s\"\n", gf_4cc_to_str(prores_frame->frame_identifier) ));
		gf_bs_seek(bs, start);
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	/*parse frame header*/
	pos = gf_bs_get_position(bs);
	prores_frame->frame_hdr_size = gf_bs_read_u16(bs);
	if (gf_bs_available(bs) + 2 < prores_frame->frame_hdr_size) {
		gf_bs_seek(bs, start);
		return GF_BUFFER_TOO_SMALL;
	}
	gf_bs_read_u8(bs);
	prores_frame->version = gf_bs_read_u8(bs);
	prores_frame->encoder_id = gf_bs_read_u32(bs);
	prores_frame->width = gf_bs_read_u16(bs);
	prores_frame->height = gf_bs_read_u16(bs);
	prores_frame->chroma_format = gf_bs_read_int(bs, 2);
	gf_bs_read_int(bs, 2);
	prores_frame->interlaced_mode = gf_bs_read_int(bs, 2);
	gf_bs_read_int(bs, 2);
	prores_frame->aspect_ratio_information = gf_bs_read_int(bs, 4);
	prores_frame->framerate_code = gf_bs_read_int(bs, 4);
	prores_frame->color_primaries = gf_bs_read_u8(bs);
	prores_frame->transfer_characteristics = gf_bs_read_u8(bs);
	prores_frame->matrix_coefficients = gf_bs_read_u8(bs);
	gf_bs_read_int(bs, 4);
	prores_frame->alpha_channel_type = gf_bs_read_int(bs, 4);
	gf_bs_read_int(bs, 14);
	prores_frame->load_luma_quant_matrix = gf_bs_read_int(bs, 1);
	prores_frame->load_chroma_quant_matrix = gf_bs_read_int(bs, 1);
	if (prores_frame->load_luma_quant_matrix) {
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				prores_frame->luma_quant_matrix[i][j] = gf_bs_read_u8(bs);
			}
		}
	}
	if (prores_frame->load_chroma_quant_matrix) {
		for (i=0; i<8; i++) {
			for (j=0; j<8; j++) {
				prores_frame->chroma_quant_matrix[i][j] = gf_bs_read_u8(bs);
			}
		}
	}
	pos = gf_bs_get_position(bs) - pos;
	if (pos != prores_frame->frame_hdr_size) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[ProRes] Invalid frame header size, expected %d got %d\n", prores_frame->frame_hdr_size, (u32) pos));
		gf_bs_seek(bs, start);
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	prores_frame->nb_pic = ((prores_frame->interlaced_mode==1) || (prores_frame->interlaced_mode==2)) ? 2 : 1;
	gf_bs_seek(bs, start);

	return GF_OK;
}