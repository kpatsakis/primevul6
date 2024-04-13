void gf_bs_get_content_no_truncate(GF_BitStream *bs, u8 **output, u32 *outSize, u32 *alloc_size)
{
	/*only in WRITE MEM mode*/
	if (!bs || bs->bsmode != GF_BITSTREAM_WRITE_DYN) return;

	if (bs->on_block_out && bs->position>bs->bytes_out) {
		bs->on_block_out(bs->usr_data, bs->original, (u32) (bs->position - bs->bytes_out) );
	}

	if (!bs->position && !bs->nbBits) {
		if (!alloc_size) {
			*output = NULL;
			gf_free(bs->original);
		} else {
			*alloc_size = (u32) bs->size;
			*output = bs->original;
		}
		*outSize = 0;
	} else {
		if (alloc_size) {
			/*Align our buffer or we're dead!*/
			gf_bs_align(bs);
			*alloc_size = (u32) bs->size;
			*outSize = (u32) bs->position;
			*output = bs->original;
		} else {
			s32 copy = BS_CutBuffer(bs);
			if (copy < 0) {
				*output = NULL;
			} else
				*output = bs->original;
			*outSize = (u32) bs->size;
		}
	}
	bs->original = NULL;
	bs->size = 0;
	bs->position = 0;
}