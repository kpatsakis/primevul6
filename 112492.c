status WAVEFile::parseInstrument(const Tag &id, uint32_t size)
{
	uint8_t baseNote;
	int8_t detune, gain;
	uint8_t lowNote, highNote, lowVelocity, highVelocity;
	uint8_t padByte;

	readU8(&baseNote);
	readS8(&detune);
	readS8(&gain);
	readU8(&lowNote);
	readU8(&highNote);
	readU8(&lowVelocity);
	readU8(&highVelocity);
	readU8(&padByte);

	return AF_SUCCEED;
}