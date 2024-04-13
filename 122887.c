u32 gf_bs_write_data(GF_BitStream *bs, const u8 *data, u32 nbBytes)
{
	/*we need some feedback for this guy...*/
	u64 begin = bs->position;
	if (!nbBytes) return 0;

	if (gf_bs_is_align(bs)) {
		switch (bs->bsmode) {
		case GF_BITSTREAM_WRITE:
			if (bs->position+nbBytes > bs->size) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("[BS] Attempt to overwrite bitstream by %d bytes\n", bs->position + nbBytes - bs->size));
				return 0;
			}
			memcpy(bs->original + bs->position, data, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case GF_BITSTREAM_WRITE_DYN:
			//if callback mode and dispatch not disabled, dispatch bytes
			if (bs->on_block_out && !bs->prevent_dispatch) {
				assert(bs->position >= bs->bytes_out);

				if (bs->position - bs->bytes_out + nbBytes <= bs->size) {
					memcpy(bs->original + bs->position - bs->bytes_out, data, nbBytes);
					bs->position += nbBytes;
				} else {
					if (bs->position > bs->bytes_out)
						bs->on_block_out(bs->usr_data, bs->original, (u32) (bs->position - bs->bytes_out) );
					if (nbBytes)
						bs->on_block_out(bs->usr_data, (char *) data, nbBytes);

					bs->position += nbBytes;
					bs->bytes_out = bs->position;
				}
				assert(bs->position >= bs->bytes_out);
				return nbBytes;
			}
			//otherwise store
			/*need to gf_realloc ...*/
			if (bs->position + nbBytes - bs->bytes_out > bs->size) {
				u32 new_size = (u32) (bs->size*2);
				if (!new_size) new_size = BS_MEM_BLOCK_ALLOC_SIZE;

				if (bs->size + nbBytes > 0xFFFFFFFF)
					return 0;

				while (new_size < (u32) ( bs->size + nbBytes))
					new_size *= 2;
				bs->original = (char*)gf_realloc(bs->original, sizeof(u32)*new_size);
				if (!bs->original)
					return 0;
				bs->size = new_size;
			}
			memcpy(bs->original + bs->position - bs->bytes_out, data, nbBytes);
			bs->position += nbBytes;
			return nbBytes;
		case GF_BITSTREAM_FILE_READ:
		case GF_BITSTREAM_FILE_WRITE:
			if (bs->cache_write) {
				//if block fits in our write cache, write it
				if (bs->buffer_written + nbBytes < bs->cache_write_size) {
					memcpy(bs->cache_write+bs->buffer_written, data, nbBytes);
					bs->buffer_written+=nbBytes;
					return nbBytes;
				}
				//otherwise flush cache and use file write
				bs_flush_write_cache(bs);
			}

			if (gf_fwrite(data, nbBytes, bs->stream) != nbBytes) return 0;

			if (bs->size == bs->position) bs->size += nbBytes;
			bs->position += nbBytes;
			return nbBytes;
		default:
			return 0;
		}
	}

	while (nbBytes) {
		gf_bs_write_int(bs, (s32) *data, 8);
		data++;
		nbBytes--;
	}
	return (u32) (bs->position - begin);
}