SWFShape_addLineStyle2(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a,
                      int flags, float miterLimit)
{
	growLineArray(shape);
	SWFShape_useVersion(shape, SWF_SHAPE4);
	shape->lines[shape->nLines] = newSWFLineStyle2(width, r, g, b, a, flags, miterLimit);
	return ++shape->nLines;
}