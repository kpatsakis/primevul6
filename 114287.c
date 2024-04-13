GF_Err gf_isom_get_media_language(GF_ISOFile *the_file, u32 trackNumber, char **lang)
{
	u32 count;
	Bool elng_found = GF_FALSE;
	GF_TrackBox *trak;
	if (!lang) {
		return GF_BAD_PARAM;
	}
	*lang = NULL;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media) return GF_BAD_PARAM;
	count = gf_list_count(trak->Media->child_boxes);
	if (count>0) {
		u32 i;
		for (i = 0; i < count; i++) {
			GF_Box *box = (GF_Box *)gf_list_get(trak->Media->child_boxes, i);
			if (box->type == GF_ISOM_BOX_TYPE_ELNG) {
				*lang = gf_strdup(((GF_ExtendedLanguageBox *)box)->extended_language);
				return GF_OK;
			}
		}
	}
	if (!elng_found) {
		*lang = gf_strdup(trak->Media->mediaHeader->packedLanguage);
	}
	return GF_OK;
}