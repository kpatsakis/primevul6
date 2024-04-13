slhc_toss(struct slcompress *comp)
{
	if ( comp == NULLSLCOMPR )
		return 0;

	comp->flags |= SLF_TOSS;
	return 0;
}