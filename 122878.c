u64 gf_bs_get_size(GF_BitStream *bs)
{
	if (bs->cache_write) {
		if (bs->size == bs->position)
			return bs->size + bs->buffer_written;
		else
			return bs->size;
	}
	if (bs->on_block_out)
		return bs->position;
	return bs->size;
}