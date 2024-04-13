
GF_Err trun_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_TrunEntry *p;
	GF_Err e;
	u32 i, count;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->sample_count);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		gf_bs_write_u32(bs, ptr->data_offset);
	}
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) {
		gf_bs_write_u32(bs, ptr->first_sample_flags);
	}

	if (! (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) ) ) {
		return GF_OK;
	}

	count = gf_list_count(ptr->entries);
	for (i=0; i<count; i++) {
		p = (GF_TrunEntry*)gf_list_get(ptr->entries, i);

		if (ptr->flags & GF_ISOM_TRUN_DURATION) {
			gf_bs_write_u32(bs, p->Duration);
		}
		if (ptr->flags & GF_ISOM_TRUN_SIZE) {
			gf_bs_write_u32(bs, p->size);
		}
		//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
		if (ptr->flags & GF_ISOM_TRUN_FLAGS) {
			gf_bs_write_u32(bs, p->flags);
		}
		if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) {
			if (ptr->version==0) {
				gf_bs_write_u32(bs, p->CTS_Offset);
			} else {
				gf_bs_write_u32(bs, (u32) p->CTS_Offset);
			}
		}
	}
	return GF_OK;