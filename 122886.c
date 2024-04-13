void gf_bs_flush(GF_BitStream *bs)
{
	if (!bs->stream) return;
	if (bs->bsmode != GF_BITSTREAM_FILE_WRITE) return;

	if (bs->cache_write)
		bs_flush_write_cache(bs);

	gf_fflush(bs->stream);
}