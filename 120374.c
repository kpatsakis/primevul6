GF_Box *cslg_New()
{
	ISOM_DECL_BOX_ALLOC(GF_CompositionToDecodeBox, GF_ISOM_BOX_TYPE_CSLG);
	return (GF_Box *) tmp;
}