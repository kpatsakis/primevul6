stack_slot_stack_type_full_name (ILStackDesc *value)
{
	GString *str = g_string_new ("");
	char *result;
	gboolean has_pred = FALSE, first = TRUE;

	if ((value->stype & TYPE_MASK) != value->stype) {
		g_string_append(str, "[");
		APPEND_WITH_PREDICATE (stack_slot_is_this_pointer, "this");
		APPEND_WITH_PREDICATE (stack_slot_is_boxed_value, "boxed");
		APPEND_WITH_PREDICATE (stack_slot_is_null_literal, "null");
		APPEND_WITH_PREDICATE (stack_slot_is_managed_mutability_pointer, "cmmp");
		APPEND_WITH_PREDICATE (stack_slot_is_managed_pointer, "mp");
		has_pred = TRUE;
	}

	if (mono_type_is_generic_argument (value->type) && !stack_slot_is_boxed_value (value)) {
		if (!has_pred)
			g_string_append(str, "[");
		if (!first)
			g_string_append (str, ", ");
		g_string_append (str, "unboxed");
		has_pred = TRUE;
	}

	if (has_pred)
		g_string_append(str, "] ");

	g_string_append (str, stack_slot_get_name (value));
	result = str->str;
	g_string_free (str, FALSE);
	return result;
}