decompileWITH(int n, SWF_ACTION *actions, int maxn)
{
	OUT_BEGIN2(SWF_ACTIONWITH);

	INDENT
	puts("with(");
	decompilePUSHPARAM(pop(),0);
	puts(")");
	println(" {" );
	decompileActions(sact->numActions, sact->Actions,gIndent+1);
	INDENT
	println("}" );

	return 1;
}