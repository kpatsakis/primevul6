static qboolean S_AL_CheckInput(int entityNum, sfxHandle_t sfx)
{
	if (entityNum < 0 || entityNum >= MAX_GENTITIES)
		Com_Error(ERR_DROP, "ERROR: S_AL_CheckInput: bad entitynum %i", entityNum);

	if (sfx < 0 || sfx >= numSfx)
	{
		Com_Printf(S_COLOR_RED "ERROR: S_AL_CheckInput: handle %i out of range\n", sfx);
		return qtrue;
	}

	return qfalse;
}