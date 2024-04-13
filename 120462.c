
GF_Err saio_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox *)s;

	if (ptr->flags & 1) {
		ptr->aux_info_type = gf_bs_read_u32(bs);
		ptr->aux_info_type_parameter = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 8);
	}
	ptr->entry_count = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->entry_count) {
		u32 i;
		if (ptr->version==0) {
			if (ptr->size < sizeof(u32)*ptr->entry_count)
			    return GF_ISOM_INVALID_FILE;
			ptr->offsets = gf_malloc(sizeof(u32)*ptr->entry_count);
			if (!ptr->offsets)
			    return GF_OUT_OF_MEM;
			for (i=0; i<ptr->entry_count; i++)
				ptr->offsets[i] = gf_bs_read_u32(bs);

			ISOM_DECREASE_SIZE(ptr, 4*ptr->entry_count);
		} else {
			if (ptr->size < sizeof(u64)*ptr->entry_count)
			    return GF_ISOM_INVALID_FILE;
			ptr->offsets_large = gf_malloc(sizeof(u64)*ptr->entry_count);
			if (!ptr->offsets_large)
			    return GF_OUT_OF_MEM;
			for (i=0; i<ptr->entry_count; i++)
				ptr->offsets_large[i] = gf_bs_read_u64(bs);
			ISOM_DECREASE_SIZE(ptr, 8*ptr->entry_count);
		}
	}
	return GF_OK;