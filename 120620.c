GF_Err hmhd_Size(GF_Box *s)
{
	GF_HintMediaHeaderBox *ptr = (GF_HintMediaHeaderBox *)s;
	ptr->size += 16;
	return GF_OK;
}