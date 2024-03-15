// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QJsonObject>

class PollEditorBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList options MEMBER m_options NOTIFY optionsChanged)
    Q_PROPERTY(bool multipleChoice MEMBER m_multipleChoice NOTIFY multipleChoiceChanged)
    Q_PROPERTY(bool hideTotals MEMBER m_hideTotals NOTIFY hideTotalsChanged)
    Q_PROPERTY(int expiresIn MEMBER m_expiresIn NOTIFY expiresInChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validityChanged)

public:
    explicit PollEditorBackend(QObject *parent = nullptr);

    Q_INVOKABLE void addOption();
    Q_INVOKABLE void removeOption(int index);
    Q_INVOKABLE void setOption(int index, const QString &name);

    bool isValid() const;

    QJsonObject toJsonObject() const;

    void copyFromOther(PollEditorBackend *other);

Q_SIGNALS:
    void optionsChanged();
    void multipleChoiceChanged();
    void hideTotalsChanged();
    void expiresInChanged();
    void validityChanged();

private:
    void checkValidity();

    QVariantList m_options;
    bool m_multipleChoice = false;
    bool m_hideTotals = false;
    int m_expiresIn = 86400; // 1 day
    bool m_valid = false;
};