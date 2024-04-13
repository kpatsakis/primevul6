decompileEXTENDS(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *baseclass;

	baseclass=pop();
#if 0
	/* It's useless to open a class body when there's no
	 * other code supporting it. */
	printf("class ");
	puts(getName(pop()));
	printf(" extends ");
	puts(getName(baseclass));
	println(" {" );
#else
	/* We'll do it with asm{} */
	println("asm {");
	println(" push '%s'", getName(pop()));
	println(" getvariable");
	println(" push '%s'", getName(baseclass));
	println(" getvariable");
	println(" extends");
	println("};");
#endif

	return 0;
}