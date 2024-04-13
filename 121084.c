BOOL rectangles_intersects(const RECTANGLE_16* r1, const RECTANGLE_16* r2)
{
	RECTANGLE_16 tmp;
	return rectangles_intersection(r1, r2, &tmp);
}