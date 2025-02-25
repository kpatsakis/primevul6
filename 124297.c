pushdup()
{
	struct _stack *t;
#ifdef DEBUG
	printf("*pushdup*\n");
#endif
	if(Stack == NULL)
	{
		SWF_warn("WARNING: pushdup on empty stack. This might be wrong!\n");
		return;
	}
	t = calloc(1,sizeof(*Stack));
	t->type = Stack->type;

	// If element is a string, perform deep copy of Stack->val->p
	if (Stack->val->Type == PUSH_STRING) {
		t->val = calloc(1, sizeof(struct SWF_ACTIONPUSHPARAM));
		*t->val = *Stack->val;

		int len = strlen(Stack->val->p.String) + 1; // NULL terminated
		t->val->p.String = calloc(len, sizeof(char));
		strcpy(t->val->p.String, Stack->val->p.String);
	} else {
		t->val =  Stack->val;
	}

	t->next = Stack;
	Stack = t;
}