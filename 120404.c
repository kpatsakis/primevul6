
GF_Err tfdt_Size(GF_Box *s)
{
	GF_TFBaseMediaDecodeTimeBox *ptr = (GF_TFBaseMediaDecodeTimeBox *)s;

	if (ptr->baseMediaDecodeTime<=0xFFFFFFFF) {
		ptr->version = 0;
		ptr->size += 4;
	} else {
		ptr->version = 1;
		ptr->size += 8;
	}
	return GF_OK;