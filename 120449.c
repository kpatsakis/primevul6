
GF_Err stri_Size(GF_Box *s)
{
	GF_SubTrackInformationBox *ptr = (GF_SubTrackInformationBox *)s;

	ptr->size += 8 + 4 * ptr->attribute_count;
	return GF_OK;