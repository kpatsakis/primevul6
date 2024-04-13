GF_Err gf_isom_set_removed_bytes(GF_ISOFile *movie, u64 bytes_removed)
{
	if (!movie || !movie->moov) return GF_BAD_PARAM;
	movie->bytes_removed = bytes_removed;
	return GF_OK;
}