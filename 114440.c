u32 gf_isom_get_mpeg4_subtype(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Box *entry=NULL;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !DescriptionIndex) return 0;

	if (trak->Media
		&& trak->Media->information
		&& trak->Media->information->sampleTable
		&& trak->Media->information->sampleTable->SampleDescription
	) {
		entry = (GF_Box*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	}
	if (!entry) return 0;

	//filter MPEG sub-types
	if (!IsMP4Description(entry->type)) return 0;
	return entry->type;
}