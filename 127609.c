SWFShape_addLineStyle(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a)
{
	growLineArray(shape);
	shape->lines[shape->nLines] = newSWFLineStyle(width, r, g, b, a);
	return ++shape->nLines;
}