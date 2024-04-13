string_for_opt(opts, val_optional)
char *opts;
boolean val_optional;
{
	char *colon, *equals;

	colon = index(opts, ':');
	equals = index(opts, '=');
	if (!colon || (equals && equals < colon)) colon = equals;

	if (!colon || !*++colon) {
		if (!val_optional) badoption(opts);
		return (char *)0;
	}
	return colon;
}