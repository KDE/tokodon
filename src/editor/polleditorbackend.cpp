// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "polleditorbackend.h"

#include <QJsonArray>

using namespace Qt::Literals::StringLiterals;

PollEditorBackend::PollEditorBackend(QObject *parent)
    : QObject(parent)
{
    m_options.push_back(QString{});
    m_options.push_back(QString{});
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

void PollEditorBackend::checkValidity()
{
    const bool isValid = std::all_of(m_options.cbegin(), m_options.cend(), [](const auto &option) {
        return !option.toString().isEmpty();
    });

    if (isValid != m_valid) {
        m_valid = isValid;
        Q_EMIT validityChanged();
    }
}
