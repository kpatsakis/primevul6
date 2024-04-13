
GF_Err trep_Size(GF_Box *s)
{
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *)s;
	ptr->size += 4;
	return GF_OK;