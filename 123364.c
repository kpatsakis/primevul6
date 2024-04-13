AcpiNsGetType (
    ACPI_NAMESPACE_NODE     *Node)
{
    ACPI_FUNCTION_TRACE (NsGetType);


    if (!Node)
    {
        ACPI_WARNING ((AE_INFO, "Null Node parameter"));
        return_UINT8 (ACPI_TYPE_ANY);
    }

    return_UINT8 (Node->Type);
}