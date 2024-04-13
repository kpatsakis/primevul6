BGD_DECLARE(void) gdImageColorDeallocate (gdImagePtr im, int color)
{
	if (im->trueColor || (color >= gdMaxColors) || (color < 0)) {
		return;
	}
	/* Mark it open. */
	im->open[color] = 1;
}