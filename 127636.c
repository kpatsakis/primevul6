SWFShape_drawScaledLineTo(SWFShape shape, int x, int y)
{
	SWFShape_drawScaledLine(shape, x-shape->xpos, y-shape->ypos);
}