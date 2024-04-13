static int addFillStyle(SWFShape shape, SWFFillStyle fill)
{
	int i;
	
	for ( i=0; i<shape->nFills; ++i )
	{
		if ( SWFFillStyle_equals(fill, shape->fills[i]) )
			return i;
	}

	if ( shape->isEnded )
		return -1;

	if ( shape->nFills%STYLE_INCREMENT == 0 )
	{
		int size = (shape->nFills+STYLE_INCREMENT) * sizeof(SWFFillStyle);
		shape->fills = (SWFFillStyle*)realloc(shape->fills, size);
	}

	shape->fills[shape->nFills] = fill;
	++shape->nFills;
	return shape->nFills;
}