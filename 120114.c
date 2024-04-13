GF_Err ccst_Size(GF_Box *s)
{
	GF_CodingConstraintsBox *ptr = (GF_CodingConstraintsBox *)s;
	ptr->size += 4;
	return GF_OK;
}