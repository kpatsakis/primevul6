SWFShape_moveScaledPenTo(SWFShape shape, int x, int y)
{
	ShapeRecord record;
	if ( shape->isEnded )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->moveToX = shape->xpos = x;
	record.record.stateChange->moveToY = shape->ypos = y;

	record.record.stateChange->flags |= SWF_SHAPE_MOVETOFLAG;

	if ( shape->nRecords == 0 ||
			 (shape->nRecords == 1 &&
				shape->records[0].type == SHAPERECORD_STATECHANGE) )
	{
		SWFRect_setBounds(SWFCharacter_getBounds(CHARACTER(shape)), x, x, y, y);
		SWFRect_setBounds(shape->edgeBounds, x, x, y, y);
	}
}