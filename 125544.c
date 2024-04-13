    void CmdAuthenticate::redactForLogging(mutablebson::Document* cmdObj) {
        namespace mmb = mutablebson;
        static const int numRedactedFields = 2;
        static const char* redactedFields[numRedactedFields] = { "key", "nonce" };
        for (int i = 0; i < numRedactedFields; ++i) {
            for (mmb::Element element = mmb::findFirstChildNamed(cmdObj->root(), redactedFields[i]);
                 element.ok();
                 element = mmb::findElementNamed(element.rightSibling(), redactedFields[i])) {

                element.setValueString("xxx");
            }
        }
    }