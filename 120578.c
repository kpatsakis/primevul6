GF_Err moov_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	e = gf_isom_box_array_read(s, bs, moov_AddBox);
	if (e) {
		return e;
	}
	else {
		if (!((GF_MovieBox *)s)->mvhd) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MovieHeaderBox\n"));
			return GF_ISOM_INVALID_FILE;
		}
	}
	return e;
}