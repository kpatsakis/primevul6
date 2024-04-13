SWFShape_end(SWFShape shape)
{
	int i;
	byte* buffer;

	if ( shape->isEnded )
		return;

	shape->isEnded = TRUE;
	
	buffer = SWFOutput_getBuffer(shape->out);
	buffer[0] =
		(SWFOutput_numBits(shape->nFills) << 4) + SWFOutput_numBits(shape->nLines);

	for ( i=0; i<shape->nRecords; ++i )
	{
		if ( i < shape->nRecords-1 ||
				 shape->records[i].type != SHAPERECORD_STATECHANGE )
		{
			SWFShape_writeShapeRecord(shape, shape->records[i], shape->out);
		}

		free(shape->records[i].record.stateChange); /* all in union are pointers */
	}

	SWFOutput_writeBits(shape->out, 0, 6); /* end tag */
	SWFOutput_byteAlign(shape->out);
		
	/* addStyleHeader creates a new output and adds the existing one after
		 itself- so even though it's called afterwards it's written before,
		 as it should be */
	if ( BLOCK(shape)->type > 0 )
	{
		switch (shape->useVersion)
		{
		case SWF_SHAPE1:
			BLOCK(shape)->type = SWF_DEFINESHAPE;
			break;
		case SWF_SHAPE2:
			BLOCK(shape)->type = SWF_DEFINESHAPE2;
			break;
		case SWF_SHAPE4:
			BLOCK(shape)->type = SWF_DEFINESHAPE4;
			break;
		}
		SWFShape_addStyleHeader(shape);
	}
	free(shape->records);
	shape->records = NULL;
	shape->nRecords = 0;
}