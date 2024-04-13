newblock(void)
{
	struct block *l;
	static const char *empty[] = { null };

	l = alloc(sizeof(struct block), ATEMP);
	l->flags = 0;
	/* TODO: could use e->area (l->area => l->areap) */
	ainit(&l->area);
	if (!e->loc) {
		l->argc = 0;
		l->argv = empty;
	} else {
		l->argc = e->loc->argc;
		l->argv = e->loc->argv;
	}
	l->exit = l->error = NULL;
	ktinit(&l->area, &l->vars, 0);
	ktinit(&l->area, &l->funs, 0);
	l->next = e->loc;
	e->loc = l;
}