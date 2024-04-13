u32 gf_media_nalu_next_start_code(const u8 *data, u32 data_len, u32 *sc_size)
{
	u32 avail = data_len;
	const u8 *cur = data;

	while (cur) {
		u32 v, bpos;
		u8 *next_zero = memchr(cur, 0, avail);
		if (!next_zero) return data_len;

		v = 0xffffff00;
		bpos = (u32)(next_zero - data) + 1;
		while (1) {
			u8 cval;
			if (bpos == (u32)data_len)
				return data_len;

			cval = data[bpos];
			v = ((v << 8) & 0xFFFFFF00) | ((u32)cval);
			bpos++;
			if (v == 0x00000001) {
				*sc_size = 4;
				return bpos - 4;
			}
			else if ((v & 0x00FFFFFF) == 0x00000001) {
				*sc_size = 3;
				return bpos - 3;
			}
			if (cval)
				break;
		}
		if (bpos >= data_len)
			break;
		cur = data + bpos;
		avail = data_len - bpos;
	}
	return data_len;
}