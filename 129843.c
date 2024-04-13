is_clause_nested (MonoExceptionClause *clause, MonoExceptionClause *nested)
{
	if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER && is_clause_inside_range (nested, clause->data.filter_offset, clause->handler_offset))
		return TRUE;
	return is_clause_inside_range (nested, clause->try_offset, clause->try_offset + clause->try_len) ||
	is_clause_inside_range (nested, clause->handler_offset, clause->handler_offset + clause->handler_len);
}