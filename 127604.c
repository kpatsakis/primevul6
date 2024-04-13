SWFShape_addLineStyle2filled(SWFShape shape, unsigned short width,
                             SWFFillStyle fill,
                             int flags, float miterLimit)
{
	growLineArray(shape);
	SWFShape_useVersion(shape, SWF_SHAPE4);
	SWFFillStyle_addDependency(fill, (SWFCharacter)shape);
	shape->lines[shape->nLines] = newSWFLineStyle2_filled(width, fill, flags, miterLimit);
	return ++shape->nLines;
}