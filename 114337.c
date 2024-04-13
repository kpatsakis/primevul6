Bool gf_isom_has_padding_bits(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;

	if (trak->Media->information->sampleTable->PaddingBits) return GF_TRUE;
	return GF_FALSE;
}