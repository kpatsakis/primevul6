void gf_media_reduce_aspect_ratio(u32 *width, u32 *height)
{
	u32 i = 0;
	u32 w = *width;
	u32 h = *height;
	while (std_par[i].w) {
		if (std_par[i].w * h == std_par[i].h * w) {
			*width = std_par[i].w;
			*height = std_par[i].h;
			return;
		}
		i++;
	}
	//not standard one, reduce by power of 2
	i = 2;
	while (1) {
		if (w <= i) return;
		if (h <= i) return;

		if (w % i) return;
		if (h % i) return;
		*width = w / i;
		*height = h / i;
		i *= 2;
	}
}