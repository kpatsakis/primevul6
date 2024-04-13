AcpiNsGetInternalNameLength (
    ACPI_NAMESTRING_INFO    *Info)
{
    const char              *NextExternalChar;
    UINT32                  i;


    ACPI_FUNCTION_ENTRY ();


    NextExternalChar = Info->ExternalName;
    Info->NumCarats = 0;
    Info->NumSegments = 0;
    Info->FullyQualified = FALSE;

    /*
     * For the internal name, the required length is 4 bytes per segment,
     * plus 1 each for RootPrefix, MultiNamePrefixOp, segment count,
     * trailing null (which is not really needed, but no there's harm in
     * putting it there)
     *
     * strlen() + 1 covers the first NameSeg, which has no path separator
     */
    if (ACPI_IS_ROOT_PREFIX (*NextExternalChar))
    {
        Info->FullyQualified = TRUE;
        NextExternalChar++;

        /* Skip redundant RootPrefix, like \\_SB.PCI0.SBRG.EC0 */

        while (ACPI_IS_ROOT_PREFIX (*NextExternalChar))
        {
            NextExternalChar++;
        }
    }
    else
    {
        /* Handle Carat prefixes */

        while (ACPI_IS_PARENT_PREFIX (*NextExternalChar))
        {
            Info->NumCarats++;
            NextExternalChar++;
        }
    }

    /*
     * Determine the number of ACPI name "segments" by counting the number of
     * path separators within the string. Start with one segment since the
     * segment count is [(# separators) + 1], and zero separators is ok.
     */
    if (*NextExternalChar)
    {
        Info->NumSegments = 1;
        for (i = 0; NextExternalChar[i]; i++)
        {
            if (ACPI_IS_PATH_SEPARATOR (NextExternalChar[i]))
            {
                Info->NumSegments++;
            }
        }
    }

    Info->Length = (ACPI_NAME_SIZE * Info->NumSegments) +
        4 + Info->NumCarats;

    Info->NextExternalChar = NextExternalChar;
}