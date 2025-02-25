GF_AV1Config *gf_isom_av1_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	if (gf_isom_get_reference_count(the_file, trackNumber, GF_ISOM_REF_TBAS)) {
		u32 ref_track;
		GF_Err e = gf_isom_get_reference(the_file, trackNumber, GF_ISOM_REF_TBAS, 1, &ref_track);
		if (e == GF_OK) {
			trackNumber = ref_track;
		}
	}
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex - 1);
	if (!entry) return NULL;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return NULL;
	if (!entry->av1_config|| !entry->av1_config->config) return NULL;
	return AV1_DuplicateConfig(entry->av1_config->config);
}