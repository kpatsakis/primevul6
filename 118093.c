bad_negation(optname, with_parameter)
const char *optname;
boolean with_parameter;
{
	pline_The("%s option may not %sbe negated.",
		optname,
		with_parameter ? "both have a value and " : "");
}