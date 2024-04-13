GF_Err mdia_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e = gf_isom_box_array_read(s, bs, mdia_AddBox);
	if (e) return e;
	if (!((GF_MediaBox *)s)->information) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaInformationBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!((GF_MediaBox *)s)->handler) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing HandlerBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!((GF_MediaBox *)s)->mediaHeader) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaHeaderBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	return GF_OK;
}