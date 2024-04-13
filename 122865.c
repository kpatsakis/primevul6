u64 gf_bs_get_position(GF_BitStream *bs)
{
	if (bs->cache_write)
		return bs->position + bs->buffer_written;
	return bs->position;
}