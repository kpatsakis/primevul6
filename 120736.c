
GF_Err stvi_Size(GF_Box *s)
{
	GF_StereoVideoBox *ptr = (GF_StereoVideoBox *)s;

	ptr->size+= 12 + ptr->sit_len;
	return GF_OK;