static void av1_add_obu_internal(GF_BitStream *bs, u64 pos, u64 obu_length, ObuType obu_type, GF_List **obu_list, AV1State *state)
{
	char block[OBU_BLOCK_SIZE];
	Bool has_size_field = 0, obu_extension_flag = 0;
	u8 temporal_id, spatial_id;
	GF_AV1_OBUArrayEntry *a = NULL;

	if (state && state->mem_mode) {
		if (!state->bs) state->bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		else gf_bs_reassign_buffer(state->bs, state->frame_obus, state->frame_obus_alloc);
	}
	else {
		GF_SAFEALLOC(a, GF_AV1_OBUArrayEntry);
		if (!a) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[AV1] Failed to allocate OBU\n"));
			return;
		}
	}

	gf_bs_seek(bs, pos);
	gf_av1_parse_obu_header(bs, &obu_type, &obu_extension_flag, &has_size_field, &temporal_id, &spatial_id);
	gf_bs_seek(bs, pos);

	if (has_size_field) {
		if (a) {
			a->obu = gf_malloc((size_t)obu_length);
			gf_bs_read_data(bs, a->obu, (u32)obu_length);
			a->obu_length = obu_length;
		}
		else {
			u32 remain = (u32)obu_length;
			while (remain) {
				u32 block_size = OBU_BLOCK_SIZE;
				if (block_size > remain) block_size = remain;
				gf_bs_read_data(bs, block, block_size);
				gf_bs_write_data(state->bs, block, block_size);
				remain -= block_size;
			}
			return;
		}
	}
	else {
		u8 i, hdr_size = obu_extension_flag ? 2 : 1;
		const u32 leb_size = (u32)gf_av1_leb128_size(obu_length);
		const u64 obu_size = obu_length - hdr_size;

		if (a) {
			a->obu = gf_malloc((size_t)obu_length + leb_size);
			a->obu_length = obu_length + leb_size;
			for (i = 0; i < hdr_size; ++i) {
				a->obu[i] = gf_bs_read_u8(bs);
				/*add size field flag*/
				if (i == 0) a->obu[0] |= 0x02;
			}
			{
				u32 out_size = 0;
				u8 *output = NULL;
				GF_BitStream *bsLeb128 = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
				/*write size field*/
				gf_av1_leb128_write(bsLeb128, obu_size);
				assert(gf_bs_get_position(bsLeb128) == leb_size);
				gf_bs_get_content(bsLeb128, &output, &out_size);
				gf_bs_del(bsLeb128);
				memcpy(a->obu + hdr_size, output, out_size);
				gf_free(output);
			}
			gf_bs_read_data(bs, a->obu + hdr_size + leb_size, (u32)(obu_size));
			assert(gf_bs_get_position(bs) == pos + obu_length);
		}
		else {
			u32 remain;
			for (i = 0; i < hdr_size; ++i) {
				u8 hdr_b = gf_bs_read_u8(bs);
				if (i == 0) hdr_b |= 0x02; /*add size field flag*/
				gf_bs_write_u8(state->bs, hdr_b);
			}
			/*add size field */
			gf_av1_leb128_write(state->bs, obu_size);
			remain = (u32)obu_length - hdr_size;
			while (remain) {
				u32 block_size = OBU_BLOCK_SIZE;
				if (block_size > remain) block_size = remain;
				gf_bs_read_data(bs, block, block_size);
				gf_bs_write_data(state->bs, block, block_size);
				remain -= block_size;
			}
			assert(gf_bs_get_position(bs) == pos + obu_length);
			return;
		}
	}
	if (!obu_list) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[AV1] internal error, no OBU list cannot add\n"));
		gf_free(a->obu);
		gf_free(a);
		return;
	}
	a->obu_type = obu_type;
	if (! *obu_list)
		*obu_list = gf_list_new();
	gf_list_add(*obu_list, a);
}