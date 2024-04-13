static void S_AL_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx)
{
	S_AL_SrcLoop(SRCPRI_ENTITY, sfx, origin, velocity, entityNum);
}