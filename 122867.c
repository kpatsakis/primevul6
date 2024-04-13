GF_BitStream *gf_bs_new_cbk_buffer(GF_Err (*on_block_out)(void *cbk, u8 *data, u32 block_size), void *usr_data, u8 *buffer, u32 buffer_size)
{
	GF_BitStream *tmp;

	if (!on_block_out) return NULL;

	tmp = (GF_BitStream *)gf_malloc(sizeof(GF_BitStream));
	if (!tmp) return NULL;
	memset(tmp, 0, sizeof(GF_BitStream));

	if (buffer && buffer_size) {
		tmp->size = buffer_size;
		tmp->original = buffer;
	} else {
		tmp->size = buffer_size ? buffer_size : 10*BS_MEM_BLOCK_ALLOC_SIZE;
		tmp->original = (char *) gf_malloc(sizeof(char) * ((u32) tmp->size));
		if (! tmp->original) {
			gf_free(tmp);
			return NULL;
		}
	}
	tmp->bsmode = GF_BITSTREAM_WRITE_DYN;
	tmp->on_block_out = on_block_out;
	tmp->usr_data = usr_data;

	return tmp;
}