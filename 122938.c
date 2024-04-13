GF_Err gf_bs_insert_data(GF_BitStream *bs, u8 *data, u32 size, u64 offset)
{
	u64 cur_r, cur_w, pos;
	u32 nb_io;

	if (bs->on_block_out) return GF_BAD_PARAM;

	pos = bs->position;
	nb_io = gf_bs_write_data(bs, data, size);
	if (nb_io != size) goto exit;

	cur_w = bs->position;
	gf_bs_seek(bs, pos);
	cur_r = pos;
	pos = cur_w;
	while (cur_r > offset) {
		u8 block[8196];
		u32 move_bytes = 8196;
		if (cur_r - offset < move_bytes)
			move_bytes = (u32) (cur_r - offset);

		gf_bs_seek(bs, cur_r - move_bytes);
		nb_io = gf_bs_read_data(bs, block, move_bytes);
		if (nb_io != move_bytes) goto exit;
		gf_bs_seek(bs, cur_w - move_bytes);
		nb_io = gf_bs_write_data(bs, block, move_bytes);
		if (nb_io != move_bytes) goto exit;
		cur_r -= move_bytes;
		cur_w -= move_bytes;
	}

	gf_bs_seek(bs, offset);
	nb_io = gf_bs_write_data(bs, data, size);
	if (nb_io != size) goto exit;

	gf_bs_seek(bs, pos);
	return GF_OK;

exit:
	gf_bs_seek(bs, pos);
	return GF_IO_ERR;
}