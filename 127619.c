SWFShape_drawScaledLine(SWFShape shape, int dx, int dy)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( dx == 0 && dy == 0 )
		return;

	record = newShapeRecord(shape, SHAPERECORD_LINETO);

	SWF_assert(SWFOutput_numSBits(dx) < 18);
	SWF_assert(SWFOutput_numSBits(dy) < 18);

	record.record.lineTo->dx = dx;
	record.record.lineTo->dy = dy;

	shape->xpos += dx;
	shape->ypos += dy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
}