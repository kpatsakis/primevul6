BGD_DECLARE(int) gdImageColorReplaceArray (gdImagePtr im, int len, int *src, int *dst)
{
	register int x, y;
	int c, *d, *base;
	int i, n = 0;

	if (len <= 0 || src == dst) {
		return 0;
	}
	if (len == 1) {
		return gdImageColorReplace(im, src[0], dst[0]);
	}
	if (overflow2(len, sizeof(int)<<1)) {
		return -1;
	}
	base = (int *)gdMalloc(len * (sizeof(int)<<1));
	if (!base) {
		return -1;
	}
	for (i = 0; i < len; i++) {
		base[(i<<1)]   = src[i];
		base[(i<<1)+1] = dst[i];
	}
	qsort(base, len, sizeof(int)<<1, colorCmp);

#define REPLACING_LOOP(pixel) do {										\
		for (y = im->cy1; y <= im->cy2; y++) {							\
			for (x = im->cx1; x <= im->cx2; x++) {						\
				c = pixel(im, x, y);									\
				if ( (d = (int *)bsearch(&c, base, len, sizeof(int)<<1, colorCmp)) ) { \
					gdImageSetPixel(im, x, y, d[1]);					\
					n++;												\
				}														\
			}															\
		}																\
	} while (0)

	if (im->trueColor) {
		REPLACING_LOOP(gdImageTrueColorPixel);
	} else {
		REPLACING_LOOP(gdImagePalettePixel);
	}

#undef REPLACING_LOOP

	gdFree(base);
	return n;
}