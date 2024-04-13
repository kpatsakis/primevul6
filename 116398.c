lookup_cset(ushort nrc_code, uchar csmask, bool enabled)
{
  static struct {
    ushort design;
    uchar cstype;  // 1: 94-character set, 2: 96-character set, 3: both
    bool free;     // does not need NRC enabling
    uchar cs;
  } csdesignations[] = {
    {'B', 1, 1, CSET_ASCII},	// ASCII
    {'A', 3, 1, CSET_GBCHR},	// UK Latin-1
    {'0', 1, 1, CSET_LINEDRW},	// DEC Special Line Drawing
    {'>', 1, 1, CSET_TECH},		// DEC Technical
    {'U', 1, 1, CSET_OEM},		// OEM Codepage 437
    {'<', 1, 1, CSET_DECSUPP},	// DEC User-preferred Supplemental (VT200)
    {CPAIR('%', '5'), 1, 1, CSET_DECSPGR},	// DEC Supplementary (VT300)
    // definitions for NRC support:
    {'4', 1, 0, CSET_NL},	// Dutch
    {'C', 1, 0, CSET_FI},	// Finnish
    {'5', 1, 0, CSET_FI},	// Finnish
    {'R', 1, 0, CSET_FR},	// French
    {'f', 1, 0, CSET_FR},	// French
    {'Q', 1, 0, CSET_CA},	// French Canadian (VT200, VT300)
    {'9', 1, 0, CSET_CA},	// French Canadian (VT200, VT300)
    {'K', 1, 0, CSET_DE},	// German
    {'Y', 1, 0, CSET_IT},	// Italian
    {'`', 1, 0, CSET_NO},	// Norwegian/Danish
    {'E', 1, 0, CSET_NO},	// Norwegian/Danish
    {'6', 1, 0, CSET_NO},	// Norwegian/Danish
    {CPAIR('%', '6'), 1, 0, CSET_PT},	// Portuguese (VT300)
    {'Z', 1, 0, CSET_ES},	// Spanish
    {'H', 1, 0, CSET_SE},	// Swedish
    {'7', 1, 0, CSET_SE},	// Swedish
    {'=', 1, 0, CSET_CH},	// Swiss
    // 96-character sets (xterm 336)
    {'L', 2, 1, CSET_ISO_Latin_Cyrillic},
    {'F', 2, 1, CSET_ISO_Greek_Supp},
    {'H', 2, 1, CSET_ISO_Hebrew},
    {'M', 2, 1, CSET_ISO_Latin_5},
    {CPAIR('"', '?'), 1, 1, CSET_DEC_Greek_Supp},
    {CPAIR('"', '4'), 1, 1, CSET_DEC_Hebrew_Supp},
    {CPAIR('%', '0'), 1, 1, CSET_DEC_Turkish_Supp},
    {CPAIR('&', '4'), 1, 1, CSET_DEC_Cyrillic},
    {CPAIR('"', '>'), 1, 0, CSET_NRCS_Greek},
    {CPAIR('%', '='), 1, 0, CSET_NRCS_Hebrew},
    {CPAIR('%', '2'), 1, 0, CSET_NRCS_Turkish},
  };
  for (uint i = 0; i < lengthof(csdesignations); i++)
    if (csdesignations[i].design == nrc_code
        && (csdesignations[i].cstype & csmask)
        && (csdesignations[i].free || enabled)
       )
    {
      return csdesignations[i].cs;
    }
  return 0;
}