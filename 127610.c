SWFShape_drawScaledCurveTo(SWFShape shape,
													 int controlx, int controly,
													 int anchorx, int anchory)
{
	SWFShape_drawScaledCurve(shape, controlx-shape->xpos, controly-shape->ypos,
													 anchorx-controlx, anchory-controly);
}