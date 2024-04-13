GF_Box *mfhd_New()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieFragmentHeaderBox, GF_ISOM_BOX_TYPE_MFHD);
	return (GF_Box *)tmp;
}