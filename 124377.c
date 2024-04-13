struct SWF_ACTIONPUSHPARAM * pop()
{
	struct _stack *t;
	struct SWF_ACTIONPUSHPARAM * ret;

#ifdef DEBUG
	printf("*pop*\n");
#endif
#ifdef DEBUGSTACK		/* continue w stack dummy */
	if( Stack == NULL ) push(newVar("// *** pop(): INTERNAL STACK ERROR FOUND ***"));
#else
	if( Stack == NULL ) SWF_error("Stack blown!! - pop");
#endif
	t=Stack;
	Stack=t->next;
	ret=t->val;
	return ret;
}