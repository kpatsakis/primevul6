void region16_uninit(REGION16* region)
{
	assert(region);

	if (region->data)
	{
		if ((region->data->size > 0) && (region->data != &empty_region))
			free(region->data);

		region->data = NULL;
	}
}