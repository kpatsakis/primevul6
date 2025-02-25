SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill)
{
	ShapeRecord record;
	int idx;

	if ( shape->isEnded || shape->isMorph )
		return;
	
	if(fill == NOFILL)
	{
		record = addStyleRecord(shape);
		record.record.stateChange->rightFill = 0;
		record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE1FLAG;
		return;
	}

	idx = getFillIdx(shape, fill);
	if(idx == 0) // fill not present in array
	{
		SWFFillStyle_addDependency(fill, (SWFCharacter)shape);
		if(addFillStyle(shape, fill) < 0)
			return;		
		idx = getFillIdx(shape, fill);
	}
	else if (idx >= 255 && shape->useVersion == SWF_SHAPE1)
	{
		SWF_error("Too many fills for SWFShape V1.\n" 
			  "Use a higher SWFShape version\n");
	}
				
	record = addStyleRecord(shape);
	record.record.stateChange->rightFill = idx;
	record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE1FLAG;
}