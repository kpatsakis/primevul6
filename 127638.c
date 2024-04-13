SWFShape_drawScaledCurve(SWFShape shape,
												 int controldx, int controldy,
												 int anchordx, int anchordy)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( controldx == 0 && controldy == 0 && anchordx == 0 && anchordy == 0 )
		return;
	
	// printf("curve %i,%i, %i, %i\n", controldx, controldy, anchordx,  anchordy);

	record = newShapeRecord(shape, SHAPERECORD_CURVETO);

	record.record.curveTo->controlx = controldx;
	record.record.curveTo->controly = controldy;
	record.record.curveTo->anchorx = anchordx;
	record.record.curveTo->anchory = anchordy;

	if ( SWFOutput_numSBits(controldx) >= 18 ||
			 SWFOutput_numSBits(controldy) >= 18 ||
			 SWFOutput_numSBits(anchordx) >= 18 ||
			 SWFOutput_numSBits(anchordy) >= 18 )
		SWF_error("Curve parameters too large");

	/* including the control point is sloppy, but safe.. */

	shape->xpos += controldx;
	shape->ypos += controldy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
	shape->xpos += anchordx;
	shape->ypos += anchordy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
}