SWFShape_addStyleHeader(SWFShape shape)
{
	SWFOutput out = newSWFOutput();
	SWFOutput_writeUInt16(out, CHARACTERID(shape));
	SWFOutput_writeRect(out, SWFCharacter_getBounds(CHARACTER(shape)));
	if(shape->useVersion == SWF_SHAPE4)
	{
		SWFOutput_writeRect(out, shape->edgeBounds);
		SWFOutput_writeUInt8(out, shape->flags);
	}
	
	SWFOutput_writeFillStyles(out, shape->fills, shape->nFills, 
		BLOCK(shape)->type, shape->edgeBounds);
	SWFOutput_writeLineStyles(out, shape->lines, shape->nLines, 
		BLOCK(shape)->type, shape->edgeBounds);
	
	/* prepend shape->out w/ shape header */
	SWFOutput_setNext(out, shape->out);
	shape->out = out;
}