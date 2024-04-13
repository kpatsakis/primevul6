void gf_bs_write_u16(GF_BitStream *bs, u32 value)
{
	assert(!bs->nbBits);
	if (bs->cache_write && (bs->buffer_written+2 < bs->cache_write_size) ) {
		bs->cache_write[bs->buffer_written] = (u8) ((value>>8)&0xff);
		bs->cache_write[bs->buffer_written+1] = (u8) ((value)&0xff);
		bs->buffer_written += 2;
	} else {
		BS_WriteByte(bs, (u8) ((value>>8)&0xff));
		BS_WriteByte(bs, (u8) ((value)&0xff));
	}
}