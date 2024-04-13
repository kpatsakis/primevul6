AcpiNsInternalizeName (
    const char              *ExternalName,
    char                    **ConvertedName)
{
    char                    *InternalName;
    ACPI_NAMESTRING_INFO    Info;
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE (NsInternalizeName);


    if ((!ExternalName)      ||
        (*ExternalName == 0) ||
        (!ConvertedName))
    {
        return_ACPI_STATUS (AE_BAD_PARAMETER);
    }

    /* Get the length of the new internal name */

    Info.ExternalName = ExternalName;
    AcpiNsGetInternalNameLength (&Info);

    /* We need a segment to store the internal  name */

    InternalName = ACPI_ALLOCATE_ZEROED (Info.Length);
    if (!InternalName)
    {
        return_ACPI_STATUS (AE_NO_MEMORY);
    }

    /* Build the name */

    Info.InternalName = InternalName;
    Status = AcpiNsBuildInternalName (&Info);
    if (ACPI_FAILURE (Status))
    {
        ACPI_FREE (InternalName);
        return_ACPI_STATUS (Status);
    }

    *ConvertedName = InternalName;
    return_ACPI_STATUS (AE_OK);
}