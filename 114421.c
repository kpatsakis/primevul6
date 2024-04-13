GF_Err gf_isom_get_chunks_infos(GF_ISOFile *movie, u32 trackNumber, u32 *dur_min, u32 *dur_avg, u32 *dur_max, u32 *size_min, u32 *size_avg, u32 *size_max)
{
	GF_TrackBox *trak;
	u32 i, k, sample_idx, dmin, dmax, smin, smax, tot_chunks;
	u64 davg, savg;
	GF_SampleToChunkBox *stsc;
	GF_TimeToSampleBox *stts;
	if (!movie || !trackNumber || !movie->moov) return GF_BAD_PARAM;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsc = trak->Media->information->sampleTable->SampleToChunk;
	stts = trak->Media->information->sampleTable->TimeToSample;
	if (!stsc || !stts) return GF_ISOM_INVALID_FILE;

	dmin = smin = (u32) -1;
	dmax = smax = 0;
	davg = savg = 0;
	sample_idx = 1;
	tot_chunks = 0;
	for (i=0; i<stsc->nb_entries; i++) {
		u32 nb_chunk = 0;
		if (stsc->entries[i].samplesPerChunk >  2*trak->Media->information->sampleTable->SampleSize->sampleCount) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] likely broken stco entry (%u samples per chunk but %u samples total)\n", stsc->entries[i].samplesPerChunk, trak->Media->information->sampleTable->SampleSize->sampleCount));
			return GF_ISOM_INVALID_FILE;
		}
		while (1) {
			u32 chunk_dur = 0;
			u32 chunk_size = 0;
			for (k=0; k<stsc->entries[i].samplesPerChunk; k++) {
				u64 dts;
				u32 dur;
				u32 size;
				stbl_GetSampleDTS_and_Duration(stts, k+sample_idx, &dts, &dur);
				chunk_dur += dur;
				stbl_GetSampleSize(trak->Media->information->sampleTable->SampleSize, k+sample_idx, &size);
				chunk_size += size;

			}
			if (dmin>chunk_dur) dmin = chunk_dur;
			if (dmax<chunk_dur) dmax = chunk_dur;
			davg += chunk_dur;
			if (smin>chunk_size) smin = chunk_size;
			if (smax<chunk_size) smax = chunk_size;
			savg += chunk_size;

			tot_chunks ++;
			sample_idx += stsc->entries[i].samplesPerChunk;
			if (i+1==stsc->nb_entries) break;
			nb_chunk ++;
			if (stsc->entries[i].firstChunk + nb_chunk == stsc->entries[i+1].firstChunk) break;
		}
	}
	if (tot_chunks) {
		davg /= tot_chunks;
		savg /= tot_chunks;
	}
	if (dur_min) *dur_min = dmin;
	if (dur_avg) *dur_avg = (u32) davg;
	if (dur_max) *dur_max = dmax;

	if (size_min) *size_min = smin;
	if (size_avg) *size_avg = (u32) savg;
	if (size_max) *size_max = smax;
	return GF_OK;
}