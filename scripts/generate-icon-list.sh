#!/usr/bin/env sh
# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

grep -hre 'icon.name: "' src | sed 's/.*icon.name: "\(.*\)"/\1/' > src/icon-list-1.txt
grep -hre 'icon.name: "' src | sed 's/.*icon.name: "\(.*\)"/\1/' > src/icon-list-2.txt
cat src/icon-list-1.txt src/icon-list-2.txt | sort | uniq > src/icon-list.txt
rm src/icon-list-1.txt src/icon-list-2.txt

if [[ `git status --porcelain` && $1 == '--ci' ]]; then
    echo Outdated icon-list.txt. Please run: bash scripts/generate-icon-list.sh
    git --no-pager diff
    exit 1
fi
