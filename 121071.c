void region16_clear(REGION16* region)
{
	assert(region);
	assert(region->data);

	if ((region->data->size > 0) && (region->data != &empty_region))
		free(region->data);

	region->data = &empty_region;
	ZeroMemory(&region->extents, sizeof(region->extents));
}