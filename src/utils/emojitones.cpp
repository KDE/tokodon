// SPDX-FileCopyrightText: None
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/emojitones.h"
#include "utils/emojimodel.h"

QMultiHash<QString, QVariant> EmojiTones::_tones = {
#include "utils/emojitones_data.h"
};
