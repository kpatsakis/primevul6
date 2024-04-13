static void region16_copy_band_with_union(RECTANGLE_16* dst,
        const RECTANGLE_16* src, const RECTANGLE_16* end,
        UINT16 newTop, UINT16 newBottom,
        const RECTANGLE_16* unionRect,
        UINT32* dstCounter,
        const RECTANGLE_16** srcPtr, RECTANGLE_16** dstPtr)
{
	UINT16 refY = src->top;
	const RECTANGLE_16* startOverlap, *endOverlap;

	/* merges a band with the given rect
	 * Input:
	 *                   unionRect
	 *               |               |
	 *               |               |
	 * ==============+===============+================================
	 *   |Item1|  |Item2| |Item3|  |Item4|    |Item5|            Band
	 * ==============+===============+================================
	 *    before     |    overlap    |          after
	 *
	 * Resulting band:
	 *   +-----+  +----------------------+    +-----+
	 *   |Item1|  |      Item2           |    |Item3|
	 *   +-----+  +----------------------+    +-----+
	 *
	 *  We first copy as-is items that are before Item2, the first overlapping
	 *  item.
	 *  Then we find the last one that overlap unionRect to agregate Item2, Item3
	 *  and Item4 to create Item2.
	 *  Finally Item5 is copied as Item3.
	 *
	 *  When no unionRect is provided, we skip the two first steps to just copy items
	 */

	if (unionRect)
	{
		/* items before unionRect */
		while ((src < end) && (src->top == refY) && (src->right < unionRect->left))
		{
			dst->top = newTop;
			dst->bottom = newBottom;
			dst->right = src->right;
			dst->left = src->left;
			src++;
			dst++;
			*dstCounter += 1;
		}

		/* treat items overlapping with unionRect */
		startOverlap = unionRect;
		endOverlap = unionRect;

		if ((src < end) && (src->top == refY) && (src->left < unionRect->left))
			startOverlap = src;

		while ((src < end) && (src->top == refY) && (src->right < unionRect->right))
		{
			src++;
		}

		if ((src < end) && (src->top == refY) && (src->left < unionRect->right))
		{
			endOverlap = src;
			src++;
		}

		dst->bottom = newBottom;
		dst->top = newTop;
		dst->left = startOverlap->left;
		dst->right = endOverlap->right;
		dst++;
		*dstCounter += 1;
	}

	/* treat remaining items on the same band */
	while ((src < end) && (src->top == refY))
	{
		dst->top = newTop;
		dst->bottom = newBottom;
		dst->right = src->right;
		dst->left = src->left;
		src++;
		dst++;
		*dstCounter += 1;
	}

	if (srcPtr)
		*srcPtr = src;

	*dstPtr = dst;
}