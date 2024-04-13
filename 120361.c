
GF_Err reftype_Size(GF_Box *s)
{
	GF_TrackReferenceTypeBox *ptr = (GF_TrackReferenceTypeBox *)s;
	if (!ptr->trackIDCount)
		ptr->size=0;
	else
		ptr->size += (ptr->trackIDCount * sizeof(u32));
	return GF_OK;