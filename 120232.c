GF_Err ctts_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	u32 sampleCount;
	GF_CompositionOffsetBox *ptr = (GF_CompositionOffsetBox *)s;

	ptr->nb_entries = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->nb_entries > ptr->size / 8) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in ctts\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->alloc_size = ptr->nb_entries;
	ptr->entries = (GF_DttsEntry *)gf_malloc(sizeof(GF_DttsEntry)*ptr->alloc_size);
	if (!ptr->entries) return GF_OUT_OF_MEM;
	sampleCount = 0;
	for (i=0; i<ptr->nb_entries; i++) {
		ptr->entries[i].sampleCount = gf_bs_read_u32(bs);
		if (ptr->version)
			ptr->entries[i].decodingOffset = gf_bs_read_int(bs, 32);
		else
			ptr->entries[i].decodingOffset = (s32) gf_bs_read_u32(bs);
		sampleCount += ptr->entries[i].sampleCount;
	}
#ifndef GPAC_DISABLE_ISOM_WRITE
	ptr->w_LastSampleNumber = sampleCount;
#endif
	return GF_OK;
}