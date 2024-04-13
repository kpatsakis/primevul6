u64 gf_bs_read_u64(GF_BitStream *bs)
{
	u64 ret;

	if (bs->cache_read && (bs->cache_read_pos+8<bs->cache_read_size) ) {
		ret = bs->cache_read[bs->cache_read_pos];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+1];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+2];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+3];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+4];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+5];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+6];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+7];
		bs->cache_read_pos+=8;
		bs->position+=8;
		return ret;
	}
	ret = gf_bs_read_u32(bs);
	ret<<=32;
	ret |= gf_bs_read_u32(bs);
	return ret;
}