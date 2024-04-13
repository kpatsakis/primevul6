u64 gf_isom_get_duration(GF_ISOFile *movie)
{
	if (!movie || !movie->moov || !movie->moov->mvhd) return 0;

	//if file was open in Write or Edit mode, recompute the duration
	//the duration of a movie is the MaxDuration of all the tracks...

#ifndef GPAC_DISABLE_ISOM_WRITE
	gf_isom_update_duration(movie);
#endif /*GPAC_DISABLE_ISOM_WRITE*/

	return movie->moov->mvhd->duration;
}