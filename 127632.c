SWFShape_drawScaledGlyph(SWFShape shape,
                         SWFFont font, unsigned short c, int size)
{
	SWFShape glyph;
	int i, vx, vy;
	if(font == NULL)
		return;
	
	glyph = SWFFont_getGlyph(font, c);
	if(glyph == NULL)
	{
		SWF_warn("SWFShape_drawScaledGlyph: no glyph for code %i found \n", c);
		return;
	}

	vx = shape->xpos;
	vy = shape->ypos;
	for(i = 0; i < glyph->nRecords; i++)
		addShapeRecord(shape, glyph->records[i], &vx, &vy, size/1024.0);
}