static int gdAlphaOverlayColor (int src, int dst, int max )
{
	dst = dst << 1;
	if( dst > max ) {
		/* in the "light" zone */
		return dst + (src << 1) - (dst * src / max) - max;
	} else {
		/* in the "dark" zone */
		return dst * src / max;
	}
}