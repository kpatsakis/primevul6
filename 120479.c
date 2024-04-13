
GF_Err sbgp_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SampleGroupBox *ptr = (GF_SampleGroupBox *)s;

	ptr->grouping_type = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->version==1) {
		ptr->grouping_type_parameter = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 4);
	}
	ptr->entry_count = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->size < sizeof(GF_SampleGroupEntry)*ptr->entry_count)
	    return GF_ISOM_INVALID_FILE;
	ptr->sample_entries = gf_malloc(sizeof(GF_SampleGroupEntry)*ptr->entry_count);
	if (!ptr->sample_entries)
	    return GF_OUT_OF_MEM;
	if (!ptr->sample_entries) return GF_IO_ERR;
	for (i=0; i<ptr->entry_count; i++) {
		ptr->sample_entries[i].sample_count = gf_bs_read_u32(bs);
		ptr->sample_entries[i].group_description_index = gf_bs_read_u32(bs);

		ISOM_DECREASE_SIZE(ptr, 8);
	}
	return GF_OK;