u32 gf_isom_get_sample_description_count(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;

	return gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
}