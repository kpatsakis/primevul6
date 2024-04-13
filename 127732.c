unspecial(const char *name)
{
	struct tbl *tp;

	tp = ktsearch(&specials, name, hash(name));
	if (tp)
		ktdelete(tp);
}