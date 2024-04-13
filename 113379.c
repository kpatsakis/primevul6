void S_AL_UpdateEntityPosition( int entityNum, const vec3_t origin )
{
	vec3_t sanOrigin;

	VectorCopy( origin, sanOrigin );
	S_AL_SanitiseVector( sanOrigin );
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES )
		Com_Error( ERR_DROP, "S_UpdateEntityPosition: bad entitynum %i", entityNum );
	VectorCopy( sanOrigin, entityList[entityNum].origin );
}