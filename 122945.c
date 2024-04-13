GF_VPConfig *gf_isom_vp_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex - 1);
	if (!entry) return NULL;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return NULL;
	if (!entry->vp_config || !entry->vp_config->config) return NULL;
	return VP_DuplicateConfig(entry->vp_config->config);
}