BGD_DECLARE(int) gdImageColorReplace (gdImagePtr im, int src, int dst)
{
	register int x, y;
	int n = 0;

	if (src == dst) {
		return 0;
	}

#define REPLACING_LOOP(pixel) do {								\
		for (y = im->cy1; y <= im->cy2; y++) {					\
			for (x = im->cx1; x <= im->cx2; x++) {				\
				if (pixel(im, x, y) == src) {					\
					gdImageSetPixel(im, x, y, dst);				\
					n++;										\
				}												\
			}													\
		}														\
	} while (0)

	if (im->trueColor) {
		REPLACING_LOOP(gdImageTrueColorPixel);
	} else {
		REPLACING_LOOP(gdImagePalettePixel);
	}

#undef REPLACING_LOOP

	return n;
}