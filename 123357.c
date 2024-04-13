AcpiNsPrintNodePathname (
    ACPI_NAMESPACE_NODE     *Node,
    const char              *Message)
{
    ACPI_BUFFER             Buffer;
    ACPI_STATUS             Status;


    if (!Node)
    {
        AcpiOsPrintf ("[NULL NAME]");
        return;
    }

    /* Convert handle to full pathname and print it (with supplied message) */

    Buffer.Length = ACPI_ALLOCATE_LOCAL_BUFFER;

    Status = AcpiNsHandleToPathname (Node, &Buffer, TRUE);
    if (ACPI_SUCCESS (Status))
    {
        if (Message)
        {
            AcpiOsPrintf ("%s ", Message);
        }

        AcpiOsPrintf ("[%s] (Node %p)", (char *) Buffer.Pointer, Node);
        ACPI_FREE (Buffer.Pointer);
    }
}