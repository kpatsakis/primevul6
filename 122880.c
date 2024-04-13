u32 gf_bs_read_data(GF_BitStream *bs, u8 *data, u32 nbBytes)
{
	u64 orig = bs->position;

	if (bs->position+nbBytes > bs->size) return 0;

	if (gf_bs_is_align(bs) ) {
		s32 bytes_read, bytes_read_cache;
		switch (bs->bsmode) {
		case GF_BITSTREAM_READ:
		case GF_BITSTREAM_WRITE:
		case GF_BITSTREAM_WRITE_DYN:
			memcpy(data, bs->original + bs->position, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case GF_BITSTREAM_FILE_READ:
		case GF_BITSTREAM_FILE_WRITE:
			if (bs->cache_write)
				bs_flush_write_cache(bs);

			bytes_read = bytes_read_cache = 0;
			if (bs->cache_read) {
				u32 csize = bs->cache_read_size-bs->cache_read_pos;
				if (csize>nbBytes) csize = nbBytes;
				memcpy(data, bs->cache_read + bs->cache_read_pos, csize);
				bs->cache_read_pos += csize;
				nbBytes -= csize;
				bytes_read_cache = csize;
			}
			if (nbBytes) {
				bytes_read = (s32) gf_fread(data + bytes_read_cache, nbBytes, bs->stream);
				if (bytes_read<0) return bytes_read_cache;
			}
			bs->position += bytes_read + bytes_read_cache;
			return bytes_read + bytes_read_cache;
		default:
			return 0;
		}
	}

	while (nbBytes-- > 0) {
		*data++ = gf_bs_read_int(bs, 8);
	}
	return (u32) (bs->position - orig);

}