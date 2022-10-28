#!/usr/bin/env sh
# SPDX-FileCopyrightText: Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: MIT

if [[ -n `grep -re 'import org.kde.kirigami 2' | grep -v Kirigami ` ]];
then
    echo "Not correct Kirigami imports, should be aliased to Kirigami"
    grep -re 'import org.kde.kirigami 2' | grep -v Kirigami
    exit 1
fi

if [[ -n `grep -re 'import QtQuick.Controls 2' | grep -v QQC2` ]];
then
    echo "Not correct QtQuick.Controls 2 imports, should be aliased to QQC2"
    grep -re 'import QtQuick.Controls 2' | grep -v QQC2
    exit 1
fi