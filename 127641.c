static int getFillIdx(SWFShape shape, SWFFillStyle fill)
{
	int i;

	for ( i=0; i<shape->nFills; ++i )
	{
		if ( SWFFillStyle_equals(fill, shape->fills[i]) )
			return (i+1);
	}
	return 0; // no fill
}