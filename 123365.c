AcpiNsGetNode (
    ACPI_NAMESPACE_NODE     *PrefixNode,
    const char              *Pathname,
    UINT32                  Flags,
    ACPI_NAMESPACE_NODE     **ReturnNode)
{
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE_PTR (NsGetNode, ACPI_CAST_PTR (char, Pathname));


    Status = AcpiUtAcquireMutex (ACPI_MTX_NAMESPACE);
    if (ACPI_FAILURE (Status))
    {
        return_ACPI_STATUS (Status);
    }

    Status = AcpiNsGetNodeUnlocked (PrefixNode, Pathname,
        Flags, ReturnNode);

    (void) AcpiUtReleaseMutex (ACPI_MTX_NAMESPACE);
    return_ACPI_STATUS (Status);
}