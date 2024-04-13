
GF_Err sdtp_Size(GF_Box *s)
{
	GF_SampleDependencyTypeBox *ptr = (GF_SampleDependencyTypeBox *)s;
	ptr->size += ptr->sampleCount;
	return GF_OK;