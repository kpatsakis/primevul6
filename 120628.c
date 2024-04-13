
GF_Err trik_Read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_TrickPlayBox *ptr = (GF_TrickPlayBox *) s;
	ptr->entry_count = (u32) ptr->size;
	ptr->entries = (GF_TrickPlayBoxEntry *) gf_malloc(ptr->entry_count * sizeof(GF_TrickPlayBoxEntry) );
	if (ptr->entries == NULL) return GF_OUT_OF_MEM;

	for (i=0; i< ptr->entry_count; i++) {
		ptr->entries[i].pic_type = gf_bs_read_int(bs, 2);
		ptr->entries[i].dependency_level = gf_bs_read_int(bs, 6);
	}
	return GF_OK;