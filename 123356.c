AcpiNsLocal (
    ACPI_OBJECT_TYPE        Type)
{
    ACPI_FUNCTION_TRACE (NsLocal);


    if (!AcpiUtValidObjectType (Type))
    {
        /* Type code out of range  */

        ACPI_WARNING ((AE_INFO, "Invalid Object Type 0x%X", Type));
        return_UINT32 (ACPI_NS_NORMAL);
    }

    return_UINT32 (AcpiGbl_NsProperties[Type] & ACPI_NS_LOCAL);
}