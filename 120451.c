
GF_Err void_Read(GF_Box *s, GF_BitStream *bs)
{
	if (s->size) return GF_ISOM_INVALID_FILE;
	return GF_OK;