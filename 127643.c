SWFShape_setLineStyle_internal(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a)
{
	int line;
		
	if ( shape->isEnded )
		return;
	
	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals(shape->lines[line], width, r, g, b, a, 0) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle(shape, width, r, g, b, a);
	else
		++line;
	
	finishSetLine(shape, line, width);
}