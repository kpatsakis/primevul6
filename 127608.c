static inline void growLineArray(SWFShape shape)
{
	int size;

	if ( shape->nLines % STYLE_INCREMENT != 0 )
		return;

	size = (shape->nLines+STYLE_INCREMENT) * sizeof(SWFLineStyle);
	shape->lines = (SWFLineStyle*)realloc(shape->lines, size);	
}