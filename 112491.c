status WAVEFile::parseFrameCount(const Tag &id, uint32_t size)
{
	Track *track = getTrack();

	uint32_t totalFrames;
	readU32(&totalFrames);

	track->totalfframes = totalFrames;

	return AF_SUCCEED;
}