static void S_AL_ClearError( qboolean quiet )
{
	int error = qalGetError();

	if( quiet )
		return;
	if(error != AL_NO_ERROR)
	{
		Com_Printf(S_COLOR_YELLOW "WARNING: unhandled AL error: %s\n",
			S_AL_ErrorMsg(error));
	}
}