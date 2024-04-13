slhc_free(struct slcompress *comp)
{
	if ( comp == NULLSLCOMPR )
		return;

	if ( comp->tstate != NULLSLSTATE )
		kfree( comp->tstate );

	if ( comp->rstate != NULLSLSTATE )
		kfree( comp->rstate );

	kfree( comp );
}