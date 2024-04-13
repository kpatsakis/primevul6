u32 gf_bs_read_u32(GF_BitStream *bs)
{
	u32 ret;
	assert(bs->nbBits==8);

	if (bs->cache_read && (bs->cache_read_pos+4<bs->cache_read_size) ) {
		ret = bs->cache_read[bs->cache_read_pos];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+1];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+2];
		ret<<=8;
		ret |= bs->cache_read[bs->cache_read_pos+3];
		bs->cache_read_pos+=4;
		bs->position+=4;
		return ret;
	}
	ret = BS_ReadByte(bs);
	ret<<=8;
	ret |= BS_ReadByte(bs);
	ret<<=8;
	ret |= BS_ReadByte(bs);
	ret<<=8;
	ret |= BS_ReadByte(bs);
	return ret;
}