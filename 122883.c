u8 gf_bs_read_bit(GF_BitStream *bs)
{
	if (bs->nbBits == 8) {
		bs->current = BS_ReadByte(bs);
		bs->nbBits = 0;
	}
#ifdef NO_OPTS
	{
		s32 ret;
		bs->current <<= 1;
		bs->nbBits++;
		ret = (bs->current & 0x100) >> 8;
		return (u8) ret;
	}
#else
	return (u8) (bs->current & bit_mask[bs->nbBits++]) ? 1 : 0;
#endif

}