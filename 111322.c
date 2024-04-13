static uint64 TIFFReadUInt64(const uint8 *value)
{
	UInt64Aligned_t result;

	result.c[0]=value[0];
	result.c[1]=value[1];
	result.c[2]=value[2];
	result.c[3]=value[3];
	result.c[4]=value[4];
	result.c[5]=value[5];
	result.c[6]=value[6];
	result.c[7]=value[7];

	return result.l;
}