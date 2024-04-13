DECtoASCII(unsigned ch)
{
    if (xtermIsDecGraphic(ch)) {
	ch = CharOf("###########+++++##-##++++|######"[ch]);
	/*           01234567890123456789012345678901 */
    }
    return ch;
}