addStyleRecord(SWFShape shape)
{
	if ( shape->nRecords > 0 &&
			 shape->records[shape->nRecords-1].type == SHAPERECORD_STATECHANGE )
	{
		return shape->records[shape->nRecords-1];
	}
	else
		return newShapeRecord(shape, SHAPERECORD_STATECHANGE);
}