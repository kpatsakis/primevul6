GF_Box *tssy_New()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeStampSynchronyBox, GF_ISOM_BOX_TYPE_TSSY);
	return (GF_Box *)tmp;
}