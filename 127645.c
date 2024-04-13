SWFShape_setLineStyle2_internal(SWFShape shape, unsigned short width,
                       byte r, byte g, byte b, byte a,
                       int flags, float miterLimit)
{
	int line;

	if ( shape->isEnded )
		return;

	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals(shape->lines[line], width, r, g, b, a, flags) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle2(shape, width, r, g, b, a, flags, miterLimit);
	else
		++line;

	finishSetLine(shape, line, width);
}