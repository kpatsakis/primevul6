static void BS_WriteByte(GF_BitStream *bs, u8 val)
{
	/*we don't allow write on READ buffers*/
	if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_FILE_READ) ) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[BS] Attempt to write on read bitstream\n"));
		return;
	}
	if (!bs->original && !bs->stream) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[BS] Attempt to write on unassigned bitstream\n"));
		return;
	}
	/*we are in MEM mode*/
	if ( (bs->bsmode == GF_BITSTREAM_WRITE) || (bs->bsmode == GF_BITSTREAM_WRITE_DYN) ) {
		//if callback mode and dispatch is not blocked, dispatch
		if (bs->on_block_out && !bs->prevent_dispatch) {
			assert(bs->position >= bs->bytes_out);
			if (bs->position - bs->bytes_out == bs->size) {
				bs->on_block_out(bs->usr_data, bs->original, (u32) (bs->position - bs->bytes_out));
				bs->bytes_out = bs->position;
			}
			if (bs->original)
				bs->original[bs->position - bs->bytes_out] = val;
			bs->position++;
			assert(bs->position >= bs->bytes_out);
			return;
		}
		//otherwise store
		if (bs->position - bs->bytes_out == bs->size) {
			/*no more space...*/
			if (bs->bsmode != GF_BITSTREAM_WRITE_DYN) return;
			/*gf_realloc if enough space...*/
			if (bs->size > 0xFFFFFFFF) return;
			bs->size = bs->size ? (bs->size * 2) : BS_MEM_BLOCK_ALLOC_SIZE;
			bs->original = (char*)gf_realloc(bs->original, (u32)bs->size);
			if (!bs->original) return;
		}
		if (bs->original)
			bs->original[bs->position - bs->bytes_out] = val;
		bs->position++;
		return;
	}
	if (bs->cache_write) {
		if (bs->buffer_written == bs->cache_write_size) {
			bs_flush_write_cache(bs);
		}
		bs->cache_write[bs->buffer_written] = val;
		bs->buffer_written++;
		if (bs->buffer_written == bs->cache_write_size) {
			bs_flush_write_cache(bs);
		}
		return;
	}
	/*we are in FILE mode, no pb for any gf_realloc...*/
	gf_fputc(val, bs->stream);

	/*check we didn't rewind the stream*/
	if (bs->size == bs->position) bs->size++;
	bs->position += 1;
}