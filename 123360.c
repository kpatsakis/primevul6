AcpiNsGetNodeUnlocked (
    ACPI_NAMESPACE_NODE     *PrefixNode,
    const char              *Pathname,
    UINT32                  Flags,
    ACPI_NAMESPACE_NODE     **ReturnNode)
{
    ACPI_GENERIC_STATE      ScopeInfo;
    ACPI_STATUS             Status;
    char                    *InternalPath;


    ACPI_FUNCTION_TRACE_PTR (NsGetNodeUnlocked, ACPI_CAST_PTR (char, Pathname));


    /* Simplest case is a null pathname */

    if (!Pathname)
    {
        *ReturnNode = PrefixNode;
        if (!PrefixNode)
        {
            *ReturnNode = AcpiGbl_RootNode;
        }

        return_ACPI_STATUS (AE_OK);
    }

    /* Quick check for a reference to the root */

    if (ACPI_IS_ROOT_PREFIX (Pathname[0]) && (!Pathname[1]))
    {
        *ReturnNode = AcpiGbl_RootNode;
        return_ACPI_STATUS (AE_OK);
    }

    /* Convert path to internal representation */

    Status = AcpiNsInternalizeName (Pathname, &InternalPath);
    if (ACPI_FAILURE (Status))
    {
        return_ACPI_STATUS (Status);
    }

    /* Setup lookup scope (search starting point) */

    ScopeInfo.Scope.Node = PrefixNode;

    /* Lookup the name in the namespace */

    Status = AcpiNsLookup (&ScopeInfo, InternalPath, ACPI_TYPE_ANY,
        ACPI_IMODE_EXECUTE, (Flags | ACPI_NS_DONT_OPEN_SCOPE),
        NULL, ReturnNode);
    if (ACPI_FAILURE (Status))
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_EXEC, "%s, %s\n",
            Pathname, AcpiFormatException (Status)));
    }

    ACPI_FREE (InternalPath);
    return_ACPI_STATUS (Status);
}