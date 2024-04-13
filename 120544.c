GF_Err cslg_Size(GF_Box *s)
{
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;

	ptr->size += 20;
	return GF_OK;
}