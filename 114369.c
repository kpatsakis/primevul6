u8 gf_isom_has_sync_points(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable) return 0;
	if (trak->Media->information->sampleTable->SyncSample) {
		if (!trak->Media->information->sampleTable->SyncSample->nb_entries) return 2;
		return 1;
	}
	return 0;
}