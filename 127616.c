SWFOutput_writeGlyphShape(SWFOutput out, SWFShape shape)
{
	unsigned char c;
	int styleDone = 0;
	int i;

	c = 1<<4;
	SWFOutput_writeUInt8(out, c);
	shape->nFills = 1;
	shape->nLines = 0;		
	for ( i=0; i<shape->nRecords; ++i )
	{
		if(!styleDone && shape->records[i].type == SHAPERECORD_STATECHANGE)
		{
			shape->records[i].record.stateChange->flags |= SWF_SHAPE_FILLSTYLE0FLAG;
			shape->records[i].record.stateChange->leftFill = 1;
			styleDone = 1;
		}	
	
		if ( i < shape->nRecords-1 ||
				 shape->records[i].type != SHAPERECORD_STATECHANGE )
		{
			SWFShape_writeShapeRecord(shape, shape->records[i], out);
		}
	}

	SWFOutput_writeBits(out, 0, 6); /* end tag */
	SWFOutput_byteAlign(out);
}