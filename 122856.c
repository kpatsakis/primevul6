u32 gf_bs_write_byte(GF_BitStream *bs, u8 byte, u32 repeat_count)
{
	if (!gf_bs_is_align(bs) || bs->cache_write) {
		u32 count = 0;
		while (count<repeat_count) {
			gf_bs_write_int(bs, byte, 8);
			count++;
		}
		return count;
	}

	switch (bs->bsmode) {
	case GF_BITSTREAM_WRITE:
		if (bs->position + repeat_count > bs->size)
			return 0;
		memset(bs->original + bs->position, byte, repeat_count);
		bs->position += repeat_count;
		return repeat_count;
	case GF_BITSTREAM_WRITE_DYN:
		/*need to gf_realloc ...*/
		if (bs->position+repeat_count> bs->size) {
			u32 new_size = (u32) (bs->size*2);
			if (!new_size) new_size = BS_MEM_BLOCK_ALLOC_SIZE;

			if (bs->size + repeat_count > 0xFFFFFFFF)
				return 0;
			while (new_size < (u32) ( bs->size + repeat_count))
				new_size *= 2;
			bs->original = (char*)gf_realloc(bs->original, sizeof(u32)*new_size);
			if (!bs->original)
				return 0;
			bs->size = new_size;
		}
		memset(bs->original + bs->position, byte, repeat_count);
		bs->position += repeat_count;
		return repeat_count;
	case GF_BITSTREAM_FILE_READ:
	case GF_BITSTREAM_FILE_WRITE:
		if (gf_fwrite(&byte, repeat_count, bs->stream) != repeat_count)
			return 0;
		if (bs->size == bs->position) bs->size += repeat_count;
		bs->position += repeat_count;
		return repeat_count;
	default:
		return 0;
	}
}