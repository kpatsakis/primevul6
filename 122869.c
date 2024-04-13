u32 gf_bs_read_u8(GF_BitStream *bs)
{
	assert(bs->nbBits==8);
	if (bs->cache_read && (bs->cache_read_pos+1<bs->cache_read_size) ) {
		u32 ret = bs->cache_read[bs->cache_read_pos];
		bs->cache_read_pos+=1;
		bs->position+=1;
		return ret;
	}

	return (u32) BS_ReadByte(bs);
}