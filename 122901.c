static GF_Err BS_SeekIntern(GF_BitStream *bs, u64 offset)
{
	u32 i;
	/*if mem, do it */
	if ((bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_WRITE) || (bs->bsmode == GF_BITSTREAM_WRITE_DYN)) {
		if (offset > 0xFFFFFFFF) return GF_IO_ERR;
		if (!bs->original) return GF_BAD_PARAM;
		/*0 for write, read will be done automatically*/
		if (offset >= bs->size) {
			if ( (bs->bsmode == GF_BITSTREAM_READ) || (bs->bsmode == GF_BITSTREAM_WRITE) ) {
				if (offset > bs->size) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CORE, ("[BS] Attempt to seek to %d after end of bitstream %d, assuming seek to end\n", offset, bs->size));
				}
				bs->position = bs->size;
				bs->nbBits = (bs->bsmode == GF_BITSTREAM_READ) ? 8 : 0;
				return GF_OK;
			}
			/*in DYN, gf_realloc ...*/
			bs->original = (char*)gf_realloc(bs->original, (u32) (offset + 1));
			if (!bs->original)
				return GF_OUT_OF_MEM;
			for (i = 0; i < (u32) (offset + 1 - bs->size); i++) {
				bs->original[bs->size + i] = 0;
			}
			bs->size = offset + 1;
		}
		bs->current = bs->original[offset];
		bs->position = offset;
		bs->nbBits = (bs->bsmode == GF_BITSTREAM_READ) ? 8 : 0;
		return GF_OK;
	}

	if (bs->cache_write)
		bs_flush_write_cache(bs);

	if (bs->cache_read) {
		bs->cache_read_pos = bs->cache_read_size;
	}

	gf_fseek(bs->stream, offset, SEEK_SET);

	bs->position = offset;
	bs->current = 0;
	/*setup NbBits so that next acccess to the buffer will trigger read/write*/
	bs->nbBits = (bs->bsmode == GF_BITSTREAM_FILE_READ) ? 8 : 0;
	return GF_OK;
}