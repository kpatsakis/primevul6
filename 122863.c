GF_BitStream *gf_bs_new(const u8 *buffer, u64 BufferSize, u32 mode)
{
	GF_BitStream *tmp = (GF_BitStream *)gf_malloc(sizeof(GF_BitStream));
	if (!tmp) return NULL;
	memset(tmp, 0, sizeof(GF_BitStream));

	tmp->original = (char*)buffer;
	tmp->size = BufferSize;

	tmp->bsmode = mode;

	switch (tmp->bsmode) {
	case GF_BITSTREAM_READ:
		tmp->nbBits = 8;
		tmp->current = 0;
		break;
	case GF_BITSTREAM_WRITE:
	case GF_BITSTREAM_WRITE_DYN:
		tmp->nbBits = 0;
		if (! buffer) {
			/*if BufferSize is specified, use it. This is typically used when AvgSize of
			some buffers is known, but some exceed it.*/
			if (BufferSize) {
				tmp->size = BufferSize;
			} else {
				tmp->size = BS_MEM_BLOCK_ALLOC_SIZE;
			}
			tmp->original = (char *) gf_malloc(sizeof(char) * ((u32) tmp->size));
			if (! tmp->original) {
				gf_free(tmp);
				return NULL;
			}
			tmp->bsmode = GF_BITSTREAM_WRITE_DYN;
		} else {
			tmp->original = (char*)buffer;
			tmp->size = BufferSize;
		}
		break;
	default:
		/*the stream constructor is not the same...*/
		gf_free(tmp);
		return NULL;
	}
	return tmp;
}