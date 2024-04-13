Bool gf_isom_has_sync_shadows(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	if (!trak->Media->information->sampleTable->ShadowSync) return GF_FALSE;
	if (gf_list_count(trak->Media->information->sampleTable->ShadowSync->entries) ) return GF_TRUE;
	return GF_FALSE;
}