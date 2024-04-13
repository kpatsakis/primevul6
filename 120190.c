
GF_Err trak_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrackBox *ptr = (GF_TrackBox *)s;
	e = gf_isom_box_array_read(s, bs, trak_AddBox);
	if (e) return e;
	gf_isom_check_sample_desc(ptr);

	if (!ptr->Header) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing TrackHeaderBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!ptr->Media) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!ptr->Media->information || !ptr->Media->information->sampleTable) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid MediaBox\n"));
		return GF_ISOM_INVALID_FILE;
	}

	for (i=0; i<gf_list_count(ptr->Media->information->sampleTable->other_boxes); i++) {
		GF_Box *a = gf_list_get(ptr->Media->information->sampleTable->other_boxes, i);
		if ((a->type ==GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
			ptr->sample_encryption = (struct __sample_encryption_box *) a;
			break;
		}
		else if (a->type == GF_ISOM_BOX_TYPE_SENC) {
			ptr->sample_encryption = (struct __sample_encryption_box *)a;
			break;
		}
	}
	return e;