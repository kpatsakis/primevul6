push(struct SWF_ACTIONPUSHPARAM *val)
{
	struct _stack *t;
#ifdef DEBUG
	printf("*push* type=%d\n",val->Type);
#endif
	t = calloc(1,sizeof(*Stack));
	t->type = val->Type;
	t->val = val;
	t->next = Stack;
	Stack = t;
}