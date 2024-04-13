clip_1d (int *x0, int *y0, int *x1, int *y1, int mindim, int maxdim)
{
	double m;			/* gradient of line */
	if (*x0 < mindim) {
		/* start of line is left of window */
		if (*x1 < mindim)		/* as is the end, so the line never cuts the window */
			return 0;
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		/* adjust x0 to be on the left boundary (ie to be zero), and y0 to match */
		*y0 -= (int)(m * (*x0 - mindim));
		*x0 = mindim;
		/* now, perhaps, adjust the far end of the line as well */
		if (*x1 > maxdim) {
			*y1 += m * (maxdim - *x1);
			*x1 = maxdim;
		}
		return 1;
	}
	if (*x0 > maxdim) {
		/* start of line is right of window -
		complement of above */
		if (*x1 > maxdim)		/* as is the end, so the line misses the window */
			return 0;
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y0 += (int)(m * (maxdim - *x0));	/* adjust so point is on the right
							   boundary */
		*x0 = maxdim;
		/* now, perhaps, adjust the end of the line */
		if (*x1 < mindim) {
			*y1 -= (int)(m * (*x1 - mindim));
			*x1 = mindim;
		}
		return 1;
	}
	/* the final case - the start of the line is inside the window */
	if (*x1 > maxdim) {
		/* other end is outside to the right */
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y1 += (int)(m * (maxdim - *x1));
		*x1 = maxdim;
		return 1;
	}
	if (*x1 < mindim) {
		/* other end is outside to the left */
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y1 -= (int)(m * (*x1 - mindim));
		*x1 = mindim;
		return 1;
	}
	/* only get here if both points are inside the window */
	return 1;
}