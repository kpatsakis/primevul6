BOOL region16_copy(REGION16* dst, const REGION16* src)
{
	assert(dst);
	assert(dst->data);
	assert(src);
	assert(src->data);

	if (dst == src)
		return TRUE;

	dst->extents = src->extents;

	if ((dst->data->size > 0) && (dst->data != &empty_region))
		free(dst->data);

	if (src->data->size == 0)
		dst->data = &empty_region;
	else
	{
		dst->data = allocateRegion(src->data->nbRects);

		if (!dst->data)
			return FALSE;

		CopyMemory(dst->data, src->data, src->data->size);
	}

	return TRUE;
}