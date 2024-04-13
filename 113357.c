void S_AL_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater )
{
	float		orientation[6];
	vec3_t	sorigin;

	VectorCopy( origin, sorigin );
	S_AL_SanitiseVector( sorigin );

	S_AL_SanitiseVector( axis[ 0 ] );
	S_AL_SanitiseVector( axis[ 1 ] );
	S_AL_SanitiseVector( axis[ 2 ] );

	orientation[0] = axis[0][0]; orientation[1] = axis[0][1]; orientation[2] = axis[0][2];
	orientation[3] = axis[2][0]; orientation[4] = axis[2][1]; orientation[5] = axis[2][2];

	lastListenerNumber = entityNum;
	VectorCopy( sorigin, lastListenerOrigin );

	// Set OpenAL listener paramaters
	qalListenerfv(AL_POSITION, (ALfloat *)sorigin);
	qalListenerfv(AL_VELOCITY, vec3_origin);
	qalListenerfv(AL_ORIENTATION, orientation);
}