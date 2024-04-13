static ShapeRecord addShapeRecord(SWFShape shape, ShapeRecord record, 
                                  int *vx, int *vy, float scale)
{
	if ( shape->nRecords % SHAPERECORD_INCREMENT == 0 )
	{
		shape->records = (ShapeRecord*) realloc(shape->records,
					 sizeof(ShapeRecord) *
					 (shape->nRecords + SHAPERECORD_INCREMENT));
	}

	switch ( record.type )
	{
		case SHAPERECORD_STATECHANGE:
		{
			StateChangeRecord change = (StateChangeRecord)
				calloc(1,sizeof(struct stateChangeRecord));
			*change = *record.record.stateChange;
			shape->records[shape->nRecords].record.stateChange = change;
			change->moveToX += shape->xpos;
			change->moveToY += shape->ypos;
			change->moveToX *= scale;
			change->moveToY *= scale;

			*vx = change->moveToX;
			*vy = change->moveToY;
			break;
		}
		case SHAPERECORD_LINETO:
		{
			LineToRecord lineTo = (LineToRecord)
				calloc(1,sizeof(struct lineToRecord));
			*lineTo = *record.record.lineTo;
			lineTo->dx *= scale;
			lineTo->dy *= scale;
			shape->records[shape->nRecords].record.lineTo = lineTo;

			*vx += lineTo->dx;
			*vy += lineTo->dy;
			SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
				 *vx, *vy, shape->lineWidth);
			SWFRect_includePoint(shape->edgeBounds, *vx, *vy, 0);
			break;
		}
		case SHAPERECORD_CURVETO:
		{
			CurveToRecord curveTo = (CurveToRecord) 
				calloc(1,sizeof(struct curveToRecord));
			*curveTo = *record.record.curveTo;
			curveTo->controlx *= scale;
			curveTo->controly *= scale;
			curveTo->anchorx *= scale;
			curveTo->anchory *= scale;
			shape->records[shape->nRecords].record.curveTo = curveTo;
			
			*vx += curveTo->controlx;
			*vy += curveTo->controly;
			SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
				 *vx, *vy, shape->lineWidth);
			SWFRect_includePoint(shape->edgeBounds, *vx, *vy, 0);
			*vx += curveTo->anchorx;
			*vy += curveTo->anchory;
			SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
				 *vx, *vy, shape->lineWidth);
			SWFRect_includePoint(shape->edgeBounds, *vx, *vy, 0);
			break;
		}
	}
	shape->records[shape->nRecords].type = record.type;
	shape->nRecords++;
	return shape->records[shape->nRecords-1];

}