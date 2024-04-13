dump_stack_state (ILCodeDesc *state) 
{
	int i;

	printf ("(%d) ", state->size);
	for (i = 0; i < state->size; ++i)
		dump_stack_value (state->stack + i);
	printf ("\n");
}