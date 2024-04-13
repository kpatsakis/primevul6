void Helper::warning(const QString &message)
{
    m_warningString = message;

    emit newWarning(message);
}