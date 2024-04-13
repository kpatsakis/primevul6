is_array_type_compatible (MonoType *target, MonoType *candidate)
{
	MonoArrayType *left = target->data.array;
	MonoArrayType *right = candidate->data.array;

	g_assert (target->type == MONO_TYPE_ARRAY);
	g_assert (candidate->type == MONO_TYPE_ARRAY);

	if (left->rank != right->rank)
		return FALSE;

	return mono_class_is_assignable_from (left->eklass, right->eklass);
}