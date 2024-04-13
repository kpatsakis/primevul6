void Helper::error(const QString &message)
{
    m_errorString = message;

    emit newError(message);
}