
GF_Err trun_Size(GF_Box *s)
{
	u32 i, count;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;

	ptr->size += 4;
	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) ptr->size += 4;
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) ptr->size += 4;

	if (! (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) ) ) {
		return GF_OK;
	}

	//if nothing to do, this will be skipped automatically
	count = gf_list_count(ptr->entries);
	for (i=0; i<count; i++) {
		if (ptr->flags & GF_ISOM_TRUN_DURATION) ptr->size += 4;
		if (ptr->flags & GF_ISOM_TRUN_SIZE) ptr->size += 4;
		//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
		if (ptr->flags & GF_ISOM_TRUN_FLAGS) ptr->size += 4;
		if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) ptr->size += 4;
	}

	return GF_OK;