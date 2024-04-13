srcHandle_t S_AL_SrcFind(int entnum, int channel)
{
	int i;
	for(i = 0; i < srcCount; i++)
	{
		if(!srcList[i].isActive)
			continue;
		if((srcList[i].entity == entnum) && (srcList[i].channel == channel))
			return i;
	}
	return -1;
}