in_any_block (MonoMethodHeader *header, guint offset)
{
	int i;
	MonoExceptionClause *clause;

	for (i = 0; i < header->num_clauses; ++i) {
		clause = &header->clauses [i];
		if (MONO_OFFSET_IN_CLAUSE (clause, offset))
			return 1;
		if (MONO_OFFSET_IN_HANDLER (clause, offset))
			return 1;
		if (MONO_OFFSET_IN_FILTER (clause, offset))
			return 1;
	}
	return 0;
}