SWFShape_hideLine(SWFShape shape)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( shape->isMorph )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->line = 0;
	record.record.stateChange->flags |= SWF_SHAPE_LINESTYLEFLAG;
}