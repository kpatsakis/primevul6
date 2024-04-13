GF_Box *tfra_New()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackFragmentRandomAccessBox, GF_ISOM_BOX_TYPE_TFRA);
	return (GF_Box *)tmp;
}