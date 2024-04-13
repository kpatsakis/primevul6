u64 gf_isom_get_track_duration(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;

#ifndef GPAC_DISABLE_ISOM_WRITE
	/*in all modes except dump recompute duration in case headers are wrong*/
	if (movie->openMode != GF_ISOM_OPEN_READ_DUMP) {
		SetTrackDuration(trak);
	}
#endif
	return trak->Header->duration;
}