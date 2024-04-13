u64 gf_bs_get_refreshed_size(GF_BitStream *bs)
{
	s64 offset;

	switch (bs->bsmode) {
	case GF_BITSTREAM_READ:
	case GF_BITSTREAM_WRITE:
		return bs->size;

	default:
		if (bs->cache_write)
			bs_flush_write_cache(bs);

		if (bs->stream) {
			offset = gf_ftell(bs->stream);
			bs->size = gf_fsize(bs->stream);
			gf_fseek(bs->stream, offset, SEEK_SET);
		}
		return bs->size;
	}
}