static void
check_array_for_usertypes (MonoArray *arr)
{
	int i;

	if (!arr)
		return;

	for (i = 0; i < mono_array_length (arr); ++i)
		RESOLVE_ARRAY_TYPE_ELEMENT (arr, i);