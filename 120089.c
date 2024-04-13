
GF_Err trgt_Size(GF_Box *s)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;

	ptr->size+= 4;
	return GF_OK;