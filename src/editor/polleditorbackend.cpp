// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "editor/polleditorbackend.h"

using namespace Qt::Literals::StringLiterals;

PollEditorBackend::PollEditorBackend(QObject *parent)
    : QObject(parent)
{
    m_options.push_back(QString{});
    m_options.push_back(QString{});
}

void PollEditorBackend::clearOptions()
{
    m_options.clear();

    checkValidity();

    Q_EMIT optionsChanged();
}

void PollEditorBackend::addOption()
{
    m_options.push_back(QString{});

    checkValidity();

    Q_EMIT optionsChanged();
}

void PollEditorBackend::removeOption(int index)
{
    m_options.removeAt(index);

    checkValidity();

    Q_EMIT optionsChanged();
}

void PollEditorBackend::setOption(int index, const QString &name)
{
    m_options[index] = name;

    checkValidity();
}

void PollEditorBackend::setMultipleChoice(bool multipleChoice)
{
    if (m_multipleChoice != multipleChoice) {
        m_multipleChoice = multipleChoice;
        Q_EMIT multipleChoiceChanged();
    }
}

bool PollEditorBackend::multipleChoice() const
{
    return m_multipleChoice;
}

void PollEditorBackend::setHideTotals(bool hideTotals)
{
    if (hideTotals != m_hideTotals) {
        m_hideTotals = hideTotals;
        Q_EMIT hideTotalsChanged();
    }
}

bool PollEditorBackend::hideTotals() const
{
    return m_hideTotals;
}

void PollEditorBackend::setExpiresIn(int expiresIn)
{
    if (m_expiresIn != expiresIn) {
        m_expiresIn = expiresIn;
        Q_EMIT expiresInChanged();
    }
}

int PollEditorBackend::expiresIn() const
{
    return m_expiresIn;
}

QJsonObject PollEditorBackend::toJsonObject() const
{
    QJsonArray optionsArray;

    for (const auto &options : m_options) {
        optionsArray.push_back(options.toString());
    }

    QJsonObject pollObj;
    pollObj["options"_L1] = optionsArray;

    pollObj["expires_in"_L1] = m_expiresIn;
    pollObj["multiple"_L1] = m_multipleChoice;
    pollObj["hide_totals"_L1] = m_hideTotals;

    return pollObj;
}

bool PollEditorBackend::isValid() const
{
    return m_valid;
}

void PollEditorBackend::copyFromOther(PollEditorBackend *other)
{
    m_options = other->m_options;
    Q_EMIT optionsChanged();

    m_multipleChoice = other->m_multipleChoice;
    Q_EMIT multipleChoiceChanged();

    m_hideTotals = other->m_hideTotals;
    Q_EMIT hideTotalsChanged();

    m_expiresIn = other->m_expiresIn;
    Q_EMIT expiresInChanged();

    checkValidity();
}

void PollEditorBackend::checkValidity()
{
    const bool isValid = std::ranges::all_of(std::as_const(m_options), [](const auto &option) {
        return !option.toString().isEmpty();
    });

    if (isValid != m_valid) {
        m_valid = isValid;
        Q_EMIT validityChanged();
    }
}

#include "moc_polleditorbackend.cpp"
