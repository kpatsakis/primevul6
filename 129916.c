compare_types_by_table_idx (MonoReflectionTypeBuilder **type1, MonoReflectionTypeBuilder **type2)
{
	if ((*type1)->table_idx < (*type2)->table_idx)
		return -1;
	else
		if ((*type1)->table_idx > (*type2)->table_idx)
			return 1;
	else
		return 0;
}