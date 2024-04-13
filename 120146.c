
GF_Err sgpd_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 entry_count;
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)s;

	p->grouping_type = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(p, 4);

	if (p->version>=1) {
		p->default_length = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(p, 4);
	}
	if (p->version>=2) {
		p->default_description_index = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(p, 4);
	}
	entry_count = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(p, 4);

	if (entry_count>p->size)
		return GF_ISOM_INVALID_FILE;

	while (entry_count) {
		void *ptr;
		u32 parsed_bytes=0;
		u32 size = p->default_length;
		if ((p->version>=1) && !size) {
			size = gf_bs_read_u32(bs);
			ISOM_DECREASE_SIZE(p, 4);
		}
		ptr = sgpd_parse_entry(p->grouping_type, bs, size, &parsed_bytes);
		//don't return an error, just stop parsing so that we skip over the sgpd box
		if (!ptr) return GF_OK;

		ISOM_DECREASE_SIZE(p, parsed_bytes);

		gf_list_add(p->group_descriptions, ptr);
		entry_count--;
	}
	return GF_OK;