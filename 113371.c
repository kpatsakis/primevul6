sfxHandle_t S_AL_RegisterSound( const char *sample, qboolean compressed )
{
	sfxHandle_t sfx = S_AL_BufferFind(sample);

	if((!knownSfx[sfx].inMemory) && (!knownSfx[sfx].isDefault))
		S_AL_BufferLoad(sfx, s_alPrecache->integer);
	knownSfx[sfx].lastUsedTime = Com_Milliseconds();

	if (knownSfx[sfx].isDefault) {
		return 0;
	}

	return sfx;
}