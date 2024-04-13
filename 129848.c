verify_clause_relationship (VerifyContext *ctx, MonoExceptionClause *clause, MonoExceptionClause *to_test)
{
	/*clause is nested*/
	if (to_test->flags == MONO_EXCEPTION_CLAUSE_FILTER && is_clause_inside_range (clause, to_test->data.filter_offset, to_test->handler_offset)) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Exception clause inside filter"));
		return;
	}

	/*wrong nesting order.*/
	if (is_clause_nested (clause, to_test)) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Nested exception clause appears after enclosing clause"));
		return;
	}

	/*mutual protection*/
	if (clause->try_offset == to_test->try_offset && clause->try_len == to_test->try_len) {
		/*handlers are not disjoint*/
		if (is_clause_in_range (to_test, HANDLER_START (clause), clause->handler_offset + clause->handler_len)) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Exception handlers overlap"));
			return;
		}
		/* handlers are not catch or filter */
		if (!IS_CATCH_OR_FILTER (clause) || !IS_CATCH_OR_FILTER (to_test)) {
			ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Exception clauses with shared protected block are neither catch or filter"));
			return;
		}
		/*OK*/
		return;
	}

	/*not completelly disjoint*/
	if ((is_clause_in_range (to_test, clause->try_offset, clause->try_offset + clause->try_len) ||
		is_clause_in_range (to_test, HANDLER_START (clause), clause->handler_offset + clause->handler_len)) && !is_clause_nested (to_test, clause))
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Exception clauses overlap"));
}