newSWFShapeFromBitmap(SWFBitmap bitmap, int flag)
{
	SWFShape shape = newSWFShape();
	SWFFillStyle fill;
	int width, height;

	if ( flag != SWFFILL_TILED_BITMAP &&
	     flag != SWFFILL_CLIPPED_BITMAP &&
	     flag != SWFFILL_NONSMOOTHED_TILED_BITMAP &&
	     flag != SWFFILL_NONSMOOTHED_CLIPPED_BITMAP)
	{
		SWF_error("Invalid bitmap fill flag");
	}

	fill = SWFShape_addBitmapFillStyle(shape, bitmap, flag);

	width = SWFBitmap_getWidth(bitmap);
	height = SWFBitmap_getHeight(bitmap);

	SWFShape_setRightFillStyle(shape, fill);

	// XXX - scale shouldn't be hardcoded! (here, or in newSWFBitmapFillStyle)
	SWFShape_drawScaledLine(shape, width * 20, 0);
	SWFShape_drawScaledLine(shape, 0, height * 20);
	SWFShape_drawScaledLine(shape, -width * 20, 0);
	SWFShape_drawScaledLine(shape, 0, -height * 20);

	return shape;
}