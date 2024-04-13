decompileSTARTDRAG(int n, SWF_ACTION *actions, int maxn)
{
	INDENT
	puts("startDrag(");
	decompilePUSHPARAM(pop(),1);
	puts(",");
	decompilePUSHPARAM(pop(),0);
	puts(",");
	decompilePUSHPARAM(pop(),0);	//
	println(");" );
	return 0;
}