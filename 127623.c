newShapeRecord(SWFShape shape, shapeRecordType type)
{
	if ( shape->nRecords % SHAPERECORD_INCREMENT == 0 )
	{
		shape->records = (ShapeRecord*) realloc(shape->records,
					 sizeof(ShapeRecord) *
					 (shape->nRecords + SHAPERECORD_INCREMENT));
	}

	switch ( type )
	{
		case SHAPERECORD_STATECHANGE:
		{
			StateChangeRecord change = (StateChangeRecord)calloc(1,sizeof(struct stateChangeRecord));
			shape->records[shape->nRecords].record.stateChange = change;
			break;
		}
		case SHAPERECORD_LINETO:
		{
			LineToRecord lineTo = (LineToRecord) calloc(1,sizeof(struct lineToRecord));
			shape->records[shape->nRecords].record.lineTo = lineTo;
			break;
		}
		case SHAPERECORD_CURVETO:
		{
			CurveToRecord curveTo = (CurveToRecord) calloc(1,sizeof(struct curveToRecord));
			shape->records[shape->nRecords].record.curveTo = curveTo;
			break;
		}
	}

	shape->records[shape->nRecords].type = type;

// this is intentional - at least one popular compiler cannot handle [shape->nRecords++]
	shape->nRecords++;
	return shape->records[shape->nRecords-1];
}