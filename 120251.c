
GF_Err trun_Read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_TrunEntry *p;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;

	//check this is a good file
	if ((ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) && (ptr->flags & GF_ISOM_TRUN_FLAGS))
		return GF_ISOM_INVALID_FILE;

	ptr->sample_count = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		ptr->data_offset = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 4);
	}
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) {
		ptr->first_sample_flags = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 4);
	}
	if (! (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) ) ) {
		GF_SAFEALLOC(p, GF_TrunEntry);
		p->nb_pack = ptr->sample_count;
		gf_list_add(ptr->entries, p);
		return GF_OK;
	}

	//read each entry (even though nothing may be written)
	for (i=0; i<ptr->sample_count; i++) {
		u32 trun_size = 0;
		p = (GF_TrunEntry *) gf_malloc(sizeof(GF_TrunEntry));
		if (!p) return GF_OUT_OF_MEM;
		memset(p, 0, sizeof(GF_TrunEntry));

		if (ptr->flags & GF_ISOM_TRUN_DURATION) {
			p->Duration = gf_bs_read_u32(bs);
			trun_size += 4;
		}
		if (ptr->flags & GF_ISOM_TRUN_SIZE) {
			p->size = gf_bs_read_u32(bs);
			trun_size += 4;
		}
		//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
		if (ptr->flags & GF_ISOM_TRUN_FLAGS) {
			p->flags = gf_bs_read_u32(bs);
			trun_size += 4;
		}
		if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) {
			if (ptr->version==0) {
				p->CTS_Offset = (u32) gf_bs_read_u32(bs);
			} else {
				p->CTS_Offset = (s32) gf_bs_read_u32(bs);
			}
		}
		gf_list_add(ptr->entries, p);
		ISOM_DECREASE_SIZE(ptr, trun_size);
	}
	return GF_OK;