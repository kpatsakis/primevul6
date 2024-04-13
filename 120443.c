GF_Err stco_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 entries;
	GF_ChunkOffsetBox *ptr = (GF_ChunkOffsetBox *)s;

	ptr->nb_entries = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);
	if (ptr->nb_entries > ptr->size / 4) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stco\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	if (ptr->nb_entries) {
		ptr->offsets = (u32 *) gf_malloc(ptr->nb_entries * sizeof(u32) );
		if (ptr->offsets == NULL) return GF_OUT_OF_MEM;
		ptr->alloc_size = ptr->nb_entries;

		for (entries = 0; entries < ptr->nb_entries; entries++) {
			ptr->offsets[entries] = gf_bs_read_u32(bs);
		}
	}
	return GF_OK;
}