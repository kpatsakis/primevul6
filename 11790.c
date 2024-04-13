_nc_color_content(SCREEN *sp, int color, int *r, int *g, int *b)
{
    int result = ERR;
    int maxcolors;

    T((T_CALLED("color_content(%p,%d,%p,%p,%p)"),
       (void *) sp,
       color,
       (void *) r,
       (void *) g,
       (void *) b));

    if (sp == 0)
	returnCode(result);

    maxcolors = MaxColors;

    if (color < 0 || !OkColorHi(color) || !sp->_coloron) {
	result = ERR;
    } else {
	int c_r, c_g, c_b;

	if (sp->_direct_color.value) {
	    rgb_bits_t *work = &(sp->_direct_color);

#define max_direct_color(name)	((1 << work->bits.name) - 1)
#define value_direct_color(max) (1000 * ((color >> bitoff) & max)) / max

	    int max_r = max_direct_color(red);
	    int max_g = max_direct_color(green);
	    int max_b = max_direct_color(blue);

	    int bitoff = 0;

	    c_b = value_direct_color(max_b);
	    bitoff += work->bits.blue;

	    c_g = value_direct_color(max_g);
	    bitoff += work->bits.green;

	    c_r = value_direct_color(max_r);

	} else {
	    c_r = sp->_color_table[color].red;
	    c_g = sp->_color_table[color].green;
	    c_b = sp->_color_table[color].blue;
	}

	if (r)
	    *r = c_r;
	if (g)
	    *g = c_g;
	if (b)
	    *b = c_b;

	TR(TRACE_ATTRS, ("...color_content(%d,%d,%d,%d)",
			 color, c_r, c_g, c_b));
	result = OK;
    }
    returnCode(result);
}