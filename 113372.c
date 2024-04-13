void S_AL_BufferUse(sfxHandle_t sfx)
{
	if(knownSfx[sfx].filename[0] == '\0')
		return;

	if((!knownSfx[sfx].inMemory) && (!knownSfx[sfx].isDefault))
		S_AL_BufferLoad(sfx, qtrue);
	knownSfx[sfx].lastUsedTime = Sys_Milliseconds();
}