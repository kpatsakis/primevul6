u32 gf_isom_get_sync_point_count(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;
	if (trak->Media->information->sampleTable->SyncSample) {
		return trak->Media->information->sampleTable->SyncSample->nb_entries;
	}
	return 0;
}