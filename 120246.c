GF_Err stsd_Size(GF_Box *s)
{
	GF_SampleDescriptionBox *ptr = (GF_SampleDescriptionBox *)s;
	ptr->size += 4;
	return GF_OK;
}