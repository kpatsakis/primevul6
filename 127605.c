static void finishSetLine(SWFShape shape, int line, unsigned short width)
{
	ShapeRecord record;
	
	if ( width == 0 )
		shape->lineWidth = 0;
	else
		shape->lineWidth = (SWFLineStyle_getWidth(shape->lines[line-1]) + 1) / 2;
	
	if ( shape->isMorph )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->line = line;
	record.record.stateChange->flags |= SWF_SHAPE_LINESTYLEFLAG;
}