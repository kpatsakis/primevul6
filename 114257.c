u64 gf_isom_get_media_data_size(GF_ISOFile *movie, u32 trackNumber)
{
	u32 i;
	u64 size;
	GF_SampleSizeBox *stsz;
	GF_TrackBox *tk = gf_isom_get_track_from_file(movie, trackNumber);
	if (!tk) return 0;
	stsz = tk->Media->information->sampleTable->SampleSize;
	if (!stsz) return 0;
	if (stsz->sampleSize) return stsz->sampleSize*stsz->sampleCount;
	size = 0;
	for (i=0; i<stsz->sampleCount; i++) size += stsz->sizes[i];
	return size;
}