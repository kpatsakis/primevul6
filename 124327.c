pushvar(struct SWF_ACTIONPUSHPARAM *val)
{
	struct _stack *t;
#ifdef DEBUG
	printf("*pushvar*\n");
#endif
	t = calloc(1,sizeof(*Stack));
	t->type = 'v'; // ???
	t->val = val;
	t->next = Stack;
	Stack = t;
}