GF_Err gf_bs_reassign_buffer(GF_BitStream *bs, const u8 *buffer, u64 BufferSize)
{
	if (!bs) return GF_BAD_PARAM;
	bs->total_bits_read = 0;
	if (bs->bsmode == GF_BITSTREAM_READ) {
		bs->original = (char*)buffer;
		bs->size = BufferSize;
		bs->position = 0;
		bs->current = 0;
		bs->nbBits = 8;
		bs->current = 0;
		bs->nb_zeros = 0;
		return GF_OK;
	}
	if (bs->bsmode==GF_BITSTREAM_WRITE) {
		if (!buffer || !BufferSize) return GF_BAD_PARAM;
		bs->original = (char*)buffer;
		bs->size = BufferSize;
		bs->position = 0;
		bs->current = 0;
		bs->nbBits = 0;
		bs->current = 0;
		return GF_OK;
	}
	if (bs->bsmode!=GF_BITSTREAM_WRITE_DYN) return GF_BAD_PARAM;
	if (bs->original) return GF_BAD_PARAM;

	bs->position = 0;
	bs->current = 0;
	bs->nbBits = 0;
	bs->current = 0;
	bs->size = BufferSize ? BufferSize : BS_MEM_BLOCK_ALLOC_SIZE;
	if (buffer) {
		bs->original = (char *) buffer;
	} else {
		bs->original = (char *) gf_malloc(sizeof(char) * ((u32) bs->size));
		if (! bs->original) {
			return GF_OUT_OF_MEM;
		}
	}
	return GF_OK;
}