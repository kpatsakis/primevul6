u32 gf_bs_read_u8_until_delimiter(GF_BitStream *bs, u8 delimiter, u8* out, u32 max_length) {
	u32 i = 0;
	char token=0;
	u64 cur_pos = gf_bs_get_position(bs);

	if (!max_length) out = NULL;

	while(gf_bs_available(bs) && (!max_length || i < max_length)) {
		gf_bs_read_data(bs, &token, 1);
		if (token == delimiter) goto found;
		if (out) out[i] = token;
		i++;
	}

	/* Delimiter not found */
	gf_bs_seek(bs, cur_pos);
	return 0;

found:
	return i;
}