ensure_stack_size (ILCodeDesc *stack, int required)
{
	int new_size = 8;
	ILStackDesc *tmp;

	if (required < stack->max_size)
		return;

	/* We don't have to worry about the exponential growth since stack_copy prune unused space */
	new_size = MAX (8, MAX (required, stack->max_size * 2));

	g_assert (new_size >= stack->size);
	g_assert (new_size >= required);

	tmp = g_new0 (ILStackDesc, new_size);
	MEM_ALLOC (sizeof (ILStackDesc) * new_size);

	if (stack->stack) {
		if (stack->size)
			memcpy (tmp, stack->stack, stack->size * sizeof (ILStackDesc));
		g_free (stack->stack);
		MEM_FREE (sizeof (ILStackDesc) * stack->max_size);
	}

	stack->stack = tmp;
	stack->max_size = new_size;
}