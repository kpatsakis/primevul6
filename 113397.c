static void S_AL_BufferUseDefault(sfxHandle_t sfx)
{
	if(sfx == default_sfx)
		Com_Error(ERR_FATAL, "Can't load default sound effect %s", knownSfx[sfx].filename);

	Com_Printf( S_COLOR_YELLOW "WARNING: Using default sound for %s\n", knownSfx[sfx].filename);
	knownSfx[sfx].isDefault = qtrue;
	knownSfx[sfx].buffer = knownSfx[default_sfx].buffer;
}