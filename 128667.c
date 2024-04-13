static int escaped_Unicode (unsigned short Unicode, char *Buf)
{
    switch (Unicode)
    {
    case 0x3C: gs_sprintf(Buf, "&lt;"); break;
    case 0x3E: gs_sprintf(Buf, "&gt;"); break;
    case 0x26: gs_sprintf(Buf, "&amp;"); break;
    case 0x22: gs_sprintf(Buf, "&quot;"); break;
    case 0x27: gs_sprintf(Buf, "&apos;"); break;
    default:
        if (Unicode >= 32 && Unicode <= 127)
            gs_sprintf(Buf, "%c", Unicode);
        else
            gs_sprintf(Buf, "&#x%x;", Unicode);
        break;
    }

    return 0;
}