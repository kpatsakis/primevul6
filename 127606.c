SWFShape_setLineStyle2filled_internal(SWFShape shape, unsigned short width,
                       SWFFillStyle fill,
                       int flags, float miterLimit)
{
	int line;

	if ( shape->isEnded )
		return;

	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals2filled(shape->lines[line], width, fill, flags) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle2filled(shape, width, fill, flags, miterLimit);
	else
		++line;

	finishSetLine(shape, line, width);
}