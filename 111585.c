const char *uc_strerror(uc_err code)
{
    switch (code) {
    default:
        return "Unknown error code";
    case UC_ERR_OK:
        return "OK (UC_ERR_OK)";
    case UC_ERR_NOMEM:
        return "No memory available or memory not present (UC_ERR_NOMEM)";
    case UC_ERR_ARCH:
        return "Invalid/unsupported architecture (UC_ERR_ARCH)";
    case UC_ERR_HANDLE:
        return "Invalid handle (UC_ERR_HANDLE)";
    case UC_ERR_MODE:
        return "Invalid mode (UC_ERR_MODE)";
    case UC_ERR_VERSION:
        return "Different API version between core & binding (UC_ERR_VERSION)";
    case UC_ERR_READ_UNMAPPED:
        return "Invalid memory read (UC_ERR_READ_UNMAPPED)";
    case UC_ERR_WRITE_UNMAPPED:
        return "Invalid memory write (UC_ERR_WRITE_UNMAPPED)";
    case UC_ERR_FETCH_UNMAPPED:
        return "Invalid memory fetch (UC_ERR_FETCH_UNMAPPED)";
    case UC_ERR_HOOK:
        return "Invalid hook type (UC_ERR_HOOK)";
    case UC_ERR_INSN_INVALID:
        return "Invalid instruction (UC_ERR_INSN_INVALID)";
    case UC_ERR_MAP:
        return "Invalid memory mapping (UC_ERR_MAP)";
    case UC_ERR_WRITE_PROT:
        return "Write to write-protected memory (UC_ERR_WRITE_PROT)";
    case UC_ERR_READ_PROT:
        return "Read from non-readable memory (UC_ERR_READ_PROT)";
    case UC_ERR_FETCH_PROT:
        return "Fetch from non-executable memory (UC_ERR_FETCH_PROT)";
    case UC_ERR_ARG:
        return "Invalid argument (UC_ERR_ARG)";
    case UC_ERR_READ_UNALIGNED:
        return "Read from unaligned memory (UC_ERR_READ_UNALIGNED)";
    case UC_ERR_WRITE_UNALIGNED:
        return "Write to unaligned memory (UC_ERR_WRITE_UNALIGNED)";
    case UC_ERR_FETCH_UNALIGNED:
        return "Fetch from unaligned memory (UC_ERR_FETCH_UNALIGNED)";
    case UC_ERR_RESOURCE:
        return "Insufficient resource (UC_ERR_RESOURCE)";
    case UC_ERR_EXCEPTION:
        return "Unhandled CPU exception (UC_ERR_EXCEPTION)";
    }
}