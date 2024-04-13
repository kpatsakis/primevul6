SWFShape_moveScaledPen(SWFShape shape, int x, int y)
{
	SWFShape_moveScaledPenTo(shape, shape->xpos+x, shape->ypos+y);
}