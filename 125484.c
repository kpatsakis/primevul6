    PromptInfo(const UChar8* textPtr, int columns) {
        promptExtraLines = 0;
        promptLastLinePosition = 0;
        promptPreviousLen = 0;
        promptScreenColumns = columns;
        Utf32String tempUnicode(textPtr);

        // strip control characters from the prompt -- we do allow newline
        UChar32* pIn = tempUnicode.get();
        UChar32* pOut = pIn;
        while (*pIn) {
            UChar32 c = *pIn;
            if ('\n' == c || !isControlChar(c)) {
                *pOut = c;
                ++pOut;
            }
            ++pIn;
        }
        *pOut = 0;
        promptChars = pOut - tempUnicode.get();
        promptText = tempUnicode;

        int x = 0;
        for (int i = 0; i < promptChars; ++i) {
            UChar32 c = promptText[i];
            if ('\n' == c) {
                x = 0;
                ++promptExtraLines;
                promptLastLinePosition = i + 1;
            } else {
                ++x;
                if (x >= promptScreenColumns) {
                    x = 0;
                    ++promptExtraLines;
                    promptLastLinePosition = i + 1;
                }
            }
        }
        promptIndentation = promptChars - promptLastLinePosition;
        promptCursorRowOffset = promptExtraLines;
    }