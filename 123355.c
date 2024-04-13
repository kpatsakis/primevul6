AcpiNsBuildInternalName (
    ACPI_NAMESTRING_INFO    *Info)
{
    UINT32                  NumSegments = Info->NumSegments;
    char                    *InternalName = Info->InternalName;
    const char              *ExternalName = Info->NextExternalChar;
    char                    *Result = NULL;
    UINT32                  i;


    ACPI_FUNCTION_TRACE (NsBuildInternalName);


    /* Setup the correct prefixes, counts, and pointers */

    if (Info->FullyQualified)
    {
        InternalName[0] = AML_ROOT_PREFIX;

        if (NumSegments <= 1)
        {
            Result = &InternalName[1];
        }
        else if (NumSegments == 2)
        {
            InternalName[1] = AML_DUAL_NAME_PREFIX;
            Result = &InternalName[2];
        }
        else
        {
            InternalName[1] = AML_MULTI_NAME_PREFIX_OP;
            InternalName[2] = (char) NumSegments;
            Result = &InternalName[3];
        }
    }
    else
    {
        /*
         * Not fully qualified.
         * Handle Carats first, then append the name segments
         */
        i = 0;
        if (Info->NumCarats)
        {
            for (i = 0; i < Info->NumCarats; i++)
            {
                InternalName[i] = AML_PARENT_PREFIX;
            }
        }

        if (NumSegments <= 1)
        {
            Result = &InternalName[i];
        }
        else if (NumSegments == 2)
        {
            InternalName[i] = AML_DUAL_NAME_PREFIX;
            Result = &InternalName[(ACPI_SIZE) i+1];
        }
        else
        {
            InternalName[i] = AML_MULTI_NAME_PREFIX_OP;
            InternalName[(ACPI_SIZE) i+1] = (char) NumSegments;
            Result = &InternalName[(ACPI_SIZE) i+2];
        }
    }

    /* Build the name (minus path separators) */

    for (; NumSegments; NumSegments--)
    {
        for (i = 0; i < ACPI_NAME_SIZE; i++)
        {
            if (ACPI_IS_PATH_SEPARATOR (*ExternalName) ||
               (*ExternalName == 0))
            {
                /* Pad the segment with underscore(s) if segment is short */

                Result[i] = '_';
            }
            else
            {
                /* Convert the character to uppercase and save it */

                Result[i] = (char) toupper ((int) *ExternalName);
                ExternalName++;
            }
        }

        /* Now we must have a path separator, or the pathname is bad */

        if (!ACPI_IS_PATH_SEPARATOR (*ExternalName) &&
            (*ExternalName != 0))
        {
            return_ACPI_STATUS (AE_BAD_PATHNAME);
        }

        /* Move on the next segment */

        ExternalName++;
        Result += ACPI_NAME_SIZE;
    }

    /* Terminate the string */

    *Result = 0;

    if (Info->FullyQualified)
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_EXEC, "Returning [%p] (abs) \"\\%s\"\n",
            InternalName, InternalName));
    }
    else
    {
        ACPI_DEBUG_PRINT ((ACPI_DB_EXEC, "Returning [%p] (rel) \"%s\"\n",
            InternalName, InternalName));
    }

    return_ACPI_STATUS (AE_OK);
}