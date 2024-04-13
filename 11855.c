rgb2hls(int r, int g, int b, int *h, int *l, int *s)
/* convert RGB to HLS system */
{
    int min, max, t;

    if ((min = g < r ? g : r) > b)
	min = b;
    if ((max = g > r ? g : r) < b)
	max = b;

    /* calculate lightness */
    *l = ((min + max) / 20);

    if (min == max) {		/* black, white and all shades of gray */
	*h = 0;
	*s = 0;
	return;
    }

    /* calculate saturation */
    if (*l < 50)
	*s = (((max - min) * 100) / (max + min));
    else
	*s = (((max - min) * 100) / (2000 - max - min));

    /* calculate hue */
    if (r == max)
	t = (120 + ((g - b) * 60) / (max - min));
    else if (g == max)
	t = (240 + ((b - r) * 60) / (max - min));
    else
	t = (360 + ((r - g) * 60) / (max - min));

    *h = (t % 360);
}