indexToCol(int *indexed, int len, int off)
{
    int col = 0;
    while (indexed[col] < len) {
	if (indexed[col] >= off)
	    break;
	++col;
    }
    return col;
}