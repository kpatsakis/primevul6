dumpRegs()
{
int i;
for(i=0;i<6;i++)
	if( regs[i] )
		printf("reg[%d] %s\n", i, getName(regs[i]));
}