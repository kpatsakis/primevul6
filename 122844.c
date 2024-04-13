void gf_bs_write_u32(GF_BitStream *bs, u32 value)
{
	assert(!bs->nbBits);
	if (bs->cache_write && (bs->buffer_written+4 < bs->cache_write_size) ) {
		bs->cache_write[bs->buffer_written] = (u8) ((value>>24)&0xff);
		bs->cache_write[bs->buffer_written+1] = (u8) ((value>>16)&0xff);
		bs->cache_write[bs->buffer_written+2] = (u8) ((value>>8)&0xff);
		bs->cache_write[bs->buffer_written+3] = (u8) ((value)&0xff);
		bs->buffer_written += 4;
	} else {
		BS_WriteByte(bs, (u8) ((value>>24)&0xff));
		BS_WriteByte(bs, (u8) ((value>>16)&0xff));
		BS_WriteByte(bs, (u8) ((value>>8)&0xff));
		BS_WriteByte(bs, (u8) ((value)&0xff));
	}
}