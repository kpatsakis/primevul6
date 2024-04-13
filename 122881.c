static void bs_flush_write_cache(GF_BitStream *bs)
{
	if (bs->buffer_written) {
		u32 nb_write;
		nb_write = (u32) gf_fwrite(bs->cache_write, bs->buffer_written, bs->stream);

		//check we didn't rewind the bitstream
		if (bs->size == bs->position)
			bs->size += nb_write;
		bs->position += nb_write;
		bs->buffer_written = 0;
	}
}