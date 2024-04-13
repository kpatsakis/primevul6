SWFShape_getLines(SWFShape shape, SWFLineStyle** lines, int* nLines)
{
	*lines = shape->lines;
	*nLines = shape->nLines;
}