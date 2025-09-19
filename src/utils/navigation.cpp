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
#if defined(HAVE_WEBVIEW) && !defined(Q_OS_ANDROID)
    return true;
#else
    return false;
#endif
}

#include "moc_navigation.cpp"