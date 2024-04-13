void gf_bs_write_u8(GF_BitStream *bs, u32 value)
{
	assert(!bs->nbBits);

	if (bs->cache_write && (bs->buffer_written+1 < bs->cache_write_size) ) {
		bs->cache_write[bs->buffer_written] = (u8) value;
		bs->buffer_written += 1;
	} else {
		BS_WriteByte(bs, (u8) value);
	}
}