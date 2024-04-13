GF_Err stss_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;

	ptr->nb_entries = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);
	if (ptr->nb_entries > ptr->size / 4) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stss\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->alloc_size = ptr->nb_entries;
	ptr->sampleNumbers = (u32 *) gf_malloc( ptr->alloc_size * sizeof(u32));
	if (ptr->sampleNumbers == NULL) return GF_OUT_OF_MEM;

	for (i = 0; i < ptr->nb_entries; i++) {
		ptr->sampleNumbers[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}