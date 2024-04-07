// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "utils/navigation.h"

Navigation &Navigation::instance()
{
    static Navigation navigation;
    return navigation;
}

bool Navigation::isDebug() const
{
#ifdef TOKODON_DEBUG
    return true;
#else
    return false;
#endif
}

bool Navigation::hasWebView() const
{
#ifdef HAVE_WEBVIEW
    return true;
#else
    return false;
#endif
}

#include "moc_navigation.cpp"