bool CConfig::Parse(CFile& file, CString& sErrorMsg) {
    CString sLine;
    unsigned int uLineNum = 0;
    CConfig* pActiveConfig = this;
    std::stack<ConfigStackEntry> ConfigStack;
    bool bCommented = false;  // support for /**/ style comments

    if (!file.Seek(0)) {
        sErrorMsg = "Could not seek to the beginning of the config.";
        return false;
    }

    while (file.ReadLine(sLine)) {
        uLineNum++;

#define ERROR(arg)                                             \
    do {                                                       \
        std::stringstream stream;                              \
        stream << "Error on line " << uLineNum << ": " << arg; \
        sErrorMsg = stream.str();                              \
        m_SubConfigs.clear();                                  \
        m_ConfigEntries.clear();                               \
        return false;                                          \
    } while (0)

        // Remove all leading spaces and trailing line endings
        sLine.TrimLeft();
        sLine.TrimRight("\r\n");

        if (bCommented || sLine.StartsWith("/*")) {
            /* Does this comment end on the same line again? */
            bCommented = (!sLine.EndsWith("*/"));

            continue;
        }

        if ((sLine.empty()) || (sLine.StartsWith("#")) ||
            (sLine.StartsWith("//"))) {
            continue;
        }

        if ((sLine.StartsWith("<")) && (sLine.EndsWith(">"))) {
            sLine.LeftChomp();
            sLine.RightChomp();
            sLine.Trim();

            CString sTag = sLine.Token(0);
            CString sValue = sLine.Token(1, true);

            sTag.Trim();
            sValue.Trim();

            if (sTag.TrimPrefix("/")) {
                if (!sValue.empty())
                    ERROR("Malformated closing tag. Expected \"</" << sTag
                                                                   << ">\".");
                if (ConfigStack.empty())
                    ERROR("Closing tag \"" << sTag << "\" which is not open.");

                const struct ConfigStackEntry& entry = ConfigStack.top();
                CConfig myConfig(entry.Config);
                CString sName(entry.sName);

                if (!sTag.Equals(entry.sTag))
                    ERROR("Closing tag \"" << sTag << "\" which is not open.");

                // This breaks entry
                ConfigStack.pop();

                if (ConfigStack.empty())
                    pActiveConfig = this;
                else
                    pActiveConfig = &ConfigStack.top().Config;

                SubConfig& conf = pActiveConfig->m_SubConfigs[sTag.AsLower()];
                SubConfig::const_iterator it = conf.find(sName);

                if (it != conf.end())
                    ERROR("Duplicate entry for tag \"" << sTag << "\" name \""
                                                       << sName << "\".");

                conf[sName] = CConfigEntry(myConfig);
            } else {
                if (sValue.empty())
                    ERROR("Empty block name at begin of block.");
                ConfigStack.push(ConfigStackEntry(sTag.AsLower(), sValue));
                pActiveConfig = &ConfigStack.top().Config;
            }

            continue;
        }

        // If we have a regular line, figure out where it goes
        CString sName = sLine.Token(0, false, "=");
        CString sValue = sLine.Token(1, true, "=");

        // Only remove the first space, people might want
        // leading spaces (e.g. in the MOTD).
        sValue.TrimPrefix(" ");

        // We don't have any names with spaces, trim all
        // leading/trailing spaces.
        sName.Trim();

        if (sName.empty() || sValue.empty()) ERROR("Malformed line");

        CString sNameLower = sName.AsLower();
        pActiveConfig->m_ConfigEntries[sNameLower].push_back(sValue);
    }

    if (bCommented) ERROR("Comment not closed at end of file.");

    if (!ConfigStack.empty()) {
        const CString& sTag = ConfigStack.top().sTag;
        ERROR(
            "Not all tags are closed at the end of the file. Inner-most open "
            "tag is \""
            << sTag << "\".");
    }

    return true;
}