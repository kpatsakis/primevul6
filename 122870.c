GF_Err gf_bs_seek(GF_BitStream *bs, u64 offset)
{
	bs->overflow_state = 0;
	if (bs->on_block_out) {
		GF_Err e;
		if (offset < bs->bytes_out) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[BS] Attempt to seek on byte range already forwarded\n"));
			return GF_BAD_PARAM;
		}
		/*warning: we allow offset = bs->size for WRITE buffers*/
		if (offset - bs->bytes_out > bs->size)
			return GF_BAD_PARAM;
		gf_bs_align(bs);
		e = BS_SeekIntern(bs, offset - bs->bytes_out);
		bs->position += bs->bytes_out;
		return e;
	}
	if (bs->cache_write)
		bs_flush_write_cache(bs);

	/*warning: we allow offset = bs->size for WRITE buffers*/
	if (offset > bs->size) return GF_BAD_PARAM;

	gf_bs_align(bs);
	return BS_SeekIntern(bs, offset);
}