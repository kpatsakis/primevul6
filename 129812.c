is_clause_in_range (MonoExceptionClause *clause, guint32 start, guint32 end)
{
	if (clause->try_offset >= start && clause->try_offset < end)
		return TRUE;
	if (HANDLER_START (clause) >= start && HANDLER_START (clause) < end)
		return TRUE;
	return FALSE;
}