void S_AL_SrcShutup( void )
{
	int i;
	for(i = 0; i < srcCount; i++)
		S_AL_SrcKill(i);
}