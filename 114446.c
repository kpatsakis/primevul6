GF_Err gf_isom_refresh_fragmented(GF_ISOFile *movie, u64 *MissingBytes, const char *new_location)
{
#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	return GF_NOT_SUPPORTED;
#else
	u64 prevsize, size;
	u32 i;
	if (!movie || !movie->movieFileMap || !movie->moov) return GF_BAD_PARAM;
	if (movie->openMode != GF_ISOM_OPEN_READ) return GF_BAD_PARAM;

	/*refresh size*/
	size = movie->movieFileMap ? gf_bs_get_size(movie->movieFileMap->bs) : 0;

	if (new_location) {
		Bool delete_map;
		GF_DataMap *previous_movie_fileMap_address = movie->movieFileMap;
		GF_Err e;

		e = gf_isom_datamap_new(new_location, NULL, GF_ISOM_DATA_MAP_READ_ONLY, &movie->movieFileMap);
		if (e) {
			movie->movieFileMap = previous_movie_fileMap_address;
			return e;
		}

		delete_map = (previous_movie_fileMap_address != NULL ? GF_TRUE: GF_FALSE);
		for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
			GF_TrackBox *trak = (GF_TrackBox *)gf_list_get(movie->moov->trackList, i);
			if (trak->Media->information->dataHandler == previous_movie_fileMap_address) {
				//reaasign for later destruction
				trak->Media->information->scalableDataHandler = movie->movieFileMap;
				//reassign for Media_GetSample function
				trak->Media->information->dataHandler = movie->movieFileMap;
			} else if (trak->Media->information->scalableDataHandler == previous_movie_fileMap_address) {
				delete_map = GF_FALSE;
			}
		}
		if (delete_map) {
			gf_isom_datamap_del(previous_movie_fileMap_address);
		}
	}

	prevsize = gf_bs_get_refreshed_size(movie->movieFileMap->bs);
	if (prevsize==size) return GF_OK;

	if (!movie->moov->mvex)
		return GF_OK;

	//ok parse root boxes
	return gf_isom_parse_movie_boxes(movie, NULL, MissingBytes, GF_TRUE);
#endif
}