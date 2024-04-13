
GF_Err trex_Size(GF_Box *s)
{
	GF_TrackExtendsBox *ptr = (GF_TrackExtendsBox *)s;
	ptr->size += 20;
	return GF_OK;