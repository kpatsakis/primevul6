void gf_bs_skip_bytes(GF_BitStream *bs, u64 nbBytes)
{
	if (!bs || !nbBytes) return;

	gf_bs_align(bs);

	/*special case for file skipping...*/
	if ((bs->bsmode == GF_BITSTREAM_FILE_WRITE) || (bs->bsmode == GF_BITSTREAM_FILE_READ)) {
		if (bs->cache_write)
			bs_flush_write_cache(bs);

		if (bs->cache_read) {
			u32 csize = bs->cache_read_size - bs->cache_read_pos;
			if (csize>=nbBytes) {
				bs->cache_read_pos += (u32) nbBytes;
				bs->position += nbBytes;
				return;
			}
			nbBytes -= csize;
			bs->position += csize;
			bs->cache_read_pos = bs->cache_read_size;
		}
		//weird msys2 bug resulting in broken seek on some files ?!?  -the bug is not happening when doing absolute seek
//		gf_fseek(bs->stream, nbBytes, SEEK_CUR);
		bs->position += nbBytes;
		if (bs->bsmode == GF_BITSTREAM_FILE_READ) {
			if (bs->position > bs->size) bs->position = bs->size;
		}
		gf_fseek(bs->stream, bs->position, SEEK_SET);
		return;
	}

	/*special case for reading*/
	if (bs->bsmode == GF_BITSTREAM_READ) {
		if (bs->remove_emul_prevention_byte) {
			while (nbBytes) {
				gf_bs_read_u8(bs);
				nbBytes--;
			}
		} else {
			bs->position += nbBytes;
		}
		return;
	}
	/*for writing we must do it this way, otherwise pb in dynamic buffers*/
	while (nbBytes) {
		gf_bs_write_int(bs, 0, 8);
		nbBytes--;
	}
}