#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

grep -hre 'icon.name: "' src | sed 's/.*icon.name: "\(.*\)"/\1/' > src/icon-list-1.txt # icon.name
grep -hre 'icon.source: "' src | sed 's/.*icon.source: "\(.*\)"/\1/' > src/icon-list-2.txt # icon.source
grep -hre 'QIcon::fromTheme(QStringLiteral("' src | sed 's/.*QIcon::fromTheme(QStringLiteral("\(.*\)")));/\1/' > src/icon-list-3.txt # C++ QIcon
grep -hre 'source: "' src | sed -n 's/.*source: "\([^.]*\)"/\1/p' > src/icon-list-4.txt # Kirigami.Icon
cat src/icon-list-1.txt src/icon-list-2.txt src/icon-list-3.txt src/icon-list-4.txt | sort | uniq > src/icon-list.txt
rm src/icon-list-1.txt src/icon-list-2.txt src/icon-list-3.txt src/icon-list-4.txt

if [[ `git status --porcelain` && $1 == '--ci' ]]; then
    echo Outdated icon-list.txt. Please run: bash scripts/generate-icon-list.sh
    git --no-pager diff
    exit 1
fi
