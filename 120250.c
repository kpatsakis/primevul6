GF_Box *mfro_New()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieFragmentRandomAccessOffsetBox, GF_ISOM_BOX_TYPE_MFRO);
	return (GF_Box *)tmp;
}