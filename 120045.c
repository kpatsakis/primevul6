GF_Err stsc_Size(GF_Box *s)
{
	GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;

	ptr->size += 4 + (12 * ptr->nb_entries);
	return GF_OK;
}