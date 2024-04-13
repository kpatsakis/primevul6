static void BS_WriteBit(GF_BitStream *bs, u32 bit)
{
	bs->current <<= 1;
	bs->current |= bit;
	if (++ bs->nbBits == 8) {
		bs->nbBits = 0;
		BS_WriteByte(bs, (u8) bs->current);
		bs->current = 0;
	}
}