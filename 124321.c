decompileCALLFRAME(int n, SWF_ACTION *actions, int maxn)
{
	INDENT
	puts("callFrame(");
	decompilePUSHPARAM(pop(),1);
	println(");");
	return 0;
}