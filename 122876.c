void gf_bs_del(GF_BitStream *bs)
{
	if (!bs) return;
	if (bs->on_block_out && bs->position>bs->bytes_out) {
		bs->on_block_out(bs->usr_data, bs->original, (u32) (bs->position - bs->bytes_out) );
	}
	/*if we are in dynamic mode (alloc done by the bitstream), free the buffer if still present*/
	if ((bs->bsmode == GF_BITSTREAM_WRITE_DYN) && bs->original) gf_free(bs->original);
	if (bs->cache_write) {
		bs_flush_write_cache(bs);
		gf_free(bs->cache_write);
	}
	if (bs->cache_read)
		gf_free(bs->cache_read);
	gf_free(bs);
}