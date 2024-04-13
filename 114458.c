GF_Err gf_isom_open_progressive_ex(const char *fileName, u64 start_range, u64 end_range, Bool enable_frag_bounds, GF_ISOFile **the_file, u64 *BytesMissing, u32 *outBoxType)
{
	GF_Err e;
	GF_ISOFile *movie;

	if (!BytesMissing || !the_file)
		return GF_BAD_PARAM;
	*BytesMissing = 0;
	*the_file = NULL;

	movie = gf_isom_new_movie();
	if (!movie) return GF_OUT_OF_MEM;

	movie->fileName = gf_strdup(fileName);
	movie->openMode = GF_ISOM_OPEN_READ;
	movie->signal_frag_bounds = enable_frag_bounds;

#ifndef GPAC_DISABLE_ISOM_WRITE
	movie->editFileMap = NULL;
	movie->finalName = NULL;
#endif /*GPAC_DISABLE_ISOM_WRITE*/

	if (!strncmp(fileName, "isobmff://", 10)) {
		movie->movieFileMap = NULL;
		e = isom_create_init_from_mem(fileName, movie);
	} else {
		//do NOT use FileMapping on incomplete files
		e = gf_isom_datamap_new(fileName, NULL, GF_ISOM_DATA_MAP_READ, &movie->movieFileMap);
		if (e) {
			gf_isom_delete_movie(movie);
			return e;
		}

		if (start_range || end_range) {
			if (end_range>start_range) {
				gf_bs_seek(movie->movieFileMap->bs, end_range+1);
				gf_bs_truncate(movie->movieFileMap->bs);
			}
			gf_bs_seek(movie->movieFileMap->bs, start_range);
		}
		e = gf_isom_parse_movie_boxes(movie, outBoxType, BytesMissing, GF_TRUE);

	}
	if (e == GF_ISOM_INCOMPLETE_FILE) {
		//if we have a moov, we're fine
		if (movie->moov) {
			*the_file = (GF_ISOFile *)movie;
			return GF_OK;
		}
		//if not, delete the movie
		gf_isom_delete_movie(movie);
		return e;
	} else if (e) {
		//if not, delete the movie
		gf_isom_delete_movie(movie);
		return e;
	}

	//OK, let's return
	*the_file = (GF_ISOFile *)movie;
	return GF_OK;
}