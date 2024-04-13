GF_Err gf_isom_get_creation_time(GF_ISOFile *movie, u64 *creationTime, u64 *modificationTime)
{
	if (!movie || !movie->moov) return GF_BAD_PARAM;

	if (creationTime) *creationTime = movie->moov->mvhd->creationTime;
	if (creationTime) *modificationTime = movie->moov->mvhd->modificationTime;
	return GF_OK;
}