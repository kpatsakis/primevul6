u32 gf_isom_get_media_subtype(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Box *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !DescriptionIndex || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable) return 0;
	entry = (GF_Box*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return 0;

	//filter MPEG sub-types
	if (IsMP4Description(entry->type)) {
		if (gf_isom_is_encrypted_entry(entry->type)) return GF_ISOM_SUBTYPE_MPEG4_CRYP;
		else return GF_ISOM_SUBTYPE_MPEG4;
	}
	if (entry->type == GF_ISOM_BOX_TYPE_GNRV) {
		return ((GF_GenericVisualSampleEntryBox *)entry)->EntryType;
	}
	else if (entry->type == GF_ISOM_BOX_TYPE_GNRA) {
		return ((GF_GenericAudioSampleEntryBox *)entry)->EntryType;
	}
	else if (entry->type == GF_ISOM_BOX_TYPE_GNRM) {
		return ((GF_GenericSampleEntryBox *)entry)->EntryType;
	}
	return entry->type;
}