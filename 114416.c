Bool gf_isom_is_track_encrypted(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	u32 i=0;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 2;
	while (1) {
		GF_Box *entry = (GF_Box*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, i);
		if (!entry) break;
		if (gf_isom_is_encrypted_entry(entry->type)) return GF_TRUE;

		if (gf_isom_is_cenc_media(the_file, trackNumber, i+1))
			return GF_TRUE;

		i++;
	}
	return GF_FALSE;
}