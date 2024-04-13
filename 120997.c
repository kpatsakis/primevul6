BOOL region16_intersects_rect(const REGION16* src, const RECTANGLE_16* arg2)
{
	const RECTANGLE_16* rect, *endPtr, *srcExtents;
	UINT32 nbRects;

	if (!src || !src->data || !arg2)
		return FALSE;

	rect = region16_rects(src, &nbRects);

	if (!nbRects)
		return FALSE;

	srcExtents = region16_extents(src);

	if (nbRects == 1)
		return rectangles_intersects(srcExtents, arg2);

	if (!rectangles_intersects(srcExtents, arg2))
		return FALSE;

	for (endPtr = rect + nbRects; (rect < endPtr) && (arg2->bottom > rect->top); rect++)
	{
		if (rectangles_intersects(rect, arg2))
			return TRUE;
	}

	return FALSE;
}