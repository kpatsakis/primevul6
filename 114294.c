GF_Err gf_isom_get_edit(GF_ISOFile *the_file, u32 trackNumber, u32 SegmentIndex, u64 *EditTime, u64 *SegmentDuration, u64 *MediaTime, GF_ISOEditType *EditMode)
{
	u32 i;
	u64 startTime;
	GF_TrackBox *trak;
	GF_EditListBox *elst;
	GF_EdtsEntry *ent;

	ent = NULL;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (!trak->editBox ||
	        !trak->editBox->editList ||
	        (SegmentIndex > gf_list_count(trak->editBox->editList->entryList)) ||
	        !SegmentIndex)
		return GF_BAD_PARAM;

	elst = trak->editBox->editList;
	startTime = 0;

	for (i = 0; i < SegmentIndex; i++) {
		ent = (GF_EdtsEntry*)gf_list_get(elst->entryList, i);
		if (i < SegmentIndex-1) startTime += ent->segmentDuration;
	}
	*EditTime = startTime;
	*SegmentDuration = ent->segmentDuration;
	if (ent->mediaTime < 0) {
		*MediaTime = 0;
		*EditMode = GF_ISOM_EDIT_EMPTY;
		return GF_OK;
	}
	if (ent->mediaRate == 0) {
		*MediaTime = ent->mediaTime;
		*EditMode = GF_ISOM_EDIT_DWELL;
		return GF_OK;
	}
	*MediaTime = ent->mediaTime;
	*EditMode = GF_ISOM_EDIT_NORMAL;
	return GF_OK;
}