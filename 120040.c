
GF_Err pdin_Size(GF_Box *s)
{
	GF_ProgressiveDownloadBox *ptr = (GF_ProgressiveDownloadBox *)s;
	ptr->size += 8*ptr->count;
	return GF_OK;