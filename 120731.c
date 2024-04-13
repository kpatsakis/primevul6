GF_Err smhd_Size(GF_Box *s)
{
	GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;

	ptr->reserved = 0;
	ptr->size += 4;
	return GF_OK;
}