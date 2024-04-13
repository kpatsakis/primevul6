static void S_AL_ScaleGain(src_t *chksrc, vec3_t origin)
{
	float distance;
	
	if(!chksrc->local)
		distance = Distance(origin, lastListenerOrigin);
		
	// If we exceed a certain distance, scale the gain linearly until the sound
	// vanishes into nothingness.
	if(!chksrc->local && (distance -= s_alMaxDistance->value) > 0)
	{
		float scaleFactor;

		if(distance >= s_alGraceDistance->value)
			scaleFactor = 0.0f;
		else
			scaleFactor = 1.0f - distance / s_alGraceDistance->value;
		
		scaleFactor *= chksrc->curGain;
		
		if(chksrc->scaleGain != scaleFactor)
		{
			chksrc->scaleGain = scaleFactor;
			S_AL_Gain(chksrc->alSource, chksrc->scaleGain);
		}
	}
	else if(chksrc->scaleGain != chksrc->curGain)
	{
		chksrc->scaleGain = chksrc->curGain;
		S_AL_Gain(chksrc->alSource, chksrc->scaleGain);
	}
}