static qboolean S_AL_BufferEvict( void )
{
	int	i, oldestBuffer = -1;
	int	oldestTime = Sys_Milliseconds( );

	for( i = 0; i < numSfx; i++ )
	{
		if( !knownSfx[ i ].filename[ 0 ] )
			continue;

		if( !knownSfx[ i ].inMemory )
			continue;

		if( knownSfx[ i ].lastUsedTime < oldestTime )
		{
			oldestTime = knownSfx[ i ].lastUsedTime;
			oldestBuffer = i;
		}
	}

	if( oldestBuffer >= 0 )
	{
		S_AL_BufferUnload( oldestBuffer );
		return qtrue;
	}
	else
		return qfalse;
}