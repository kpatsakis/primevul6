class_of(LineData *ld, CELL *cell)
{
    CELL temp = *cell;
    int result = 0;

#if OPT_DEC_CHRSET
    if (CSET_DOUBLE(GetLineDblCS(ld))) {
	temp.col /= 2;
    }
#endif
    if (temp.col < (int) ld->lineSize)
	result = CharacterClass((int) (ld->charData[temp.col]));
    return result;
}