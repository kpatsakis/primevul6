GF_Err gf_isom_write(GF_ISOFile *movie) {
	GF_Err e;
	if (movie == NULL) return GF_ISOM_INVALID_FILE;
	e = GF_OK;

#ifndef GPAC_DISABLE_ISOM_WRITE
	//write our movie to the file
	if ((movie->openMode != GF_ISOM_OPEN_READ) && (movie->openMode != GF_ISOM_OPEN_READ_EDIT)) {
		gf_isom_get_duration(movie);
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		//movie fragment mode, just store the fragment
		if ( (movie->openMode == GF_ISOM_OPEN_WRITE) && (movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY) ) {
			e = gf_isom_close_fragments(movie);
			if (e) return e;
			//in case of mfra box usage -> create mfro, calculate box sizes and write it out
			if (movie->mfra) {
				if (!movie->mfra->mfro) {
					movie->mfra->mfro = (GF_MovieFragmentRandomAccessOffsetBox *)gf_isom_box_new_parent(&movie->mfra->child_boxes, GF_ISOM_BOX_TYPE_MFRO);
					if (!movie->mfra->mfro) return GF_OUT_OF_MEM;
				}
				e = gf_isom_box_size((GF_Box *)movie->mfra);
				if (e) return e;
				movie->mfra->mfro->container_size = (u32) movie->mfra->size;

				//write mfra
				if (!strcmp(movie->fileName, "_gpac_isobmff_redirect") && movie->on_block_out) {
					GF_BitStream *bs = gf_bs_new_cbk(movie->on_block_out, movie->on_block_out_usr_data, movie->on_block_out_block_size);

					e = gf_isom_box_write((GF_Box *)movie->mfra, bs);
					gf_bs_del(bs);
				} else {
					e = gf_isom_box_write((GF_Box *)movie->mfra, movie->editFileMap->bs);
				}
			}
		} else
#endif
			e = WriteToFile(movie, GF_FALSE);
	}
#endif /*GPAC_DISABLE_ISOM_WRITE*/

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (movie->moov) {
		u32 i;
		for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
			GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
			/*delete any pending dataHandler of scalable enhancements*/
			if (trak->Media && trak->Media->information && trak->Media->information->scalableDataHandler && (trak->Media->information->scalableDataHandler != movie->movieFileMap))
				gf_isom_datamap_del(trak->Media->information->scalableDataHandler);
		}
	}
#endif

	return e;
}