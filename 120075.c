
GF_Err lsrc_Size(GF_Box *s)
{
	GF_LASERConfigurationBox *ptr = (GF_LASERConfigurationBox *)s;
	ptr->size += ptr->hdr_size;
	return GF_OK;