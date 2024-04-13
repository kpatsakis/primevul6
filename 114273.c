Bool gf_isom_is_self_contained(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	return Media_IsSelfContained(trak->Media, sampleDescriptionIndex);
}