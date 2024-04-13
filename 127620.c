destroySWFShape(SWFShape shape)
{
	int i;
	if(shape->fills != NULL)
	{
		// Fills have to be destroyed by users. 
		/*
		for ( i=0; i<shape->nFills; ++i )
			destroySWFFillStyle(shape->fills[i]);
		*/
		free(shape->fills);
	}
	if(shape->records != NULL)
	{
		for(i = 0; i < shape->nRecords; i++)
		{
			free(shape->records[i].record.stateChange);
		}
	 	free(shape->records);
	}

	if(shape->edgeBounds != NULL)
		free(shape->edgeBounds);

	for ( i=0; i<shape->nLines; ++i )
		free(shape->lines[i]);

	if ( shape->lines != NULL )
		free(shape->lines);

	destroySWFOutput(shape->out);

#if TRACK_ALLOCS
	ming_gc_remove_node(shape->gcnode);
#endif

	destroySWFCharacter((SWFCharacter) shape);
}