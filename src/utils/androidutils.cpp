// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "androidutils.h"

#include <QCoreApplication>
#include <QJniObject>

#include <QDebug>
#include <QUrl>

#include "../network/networkcontroller.h"

AndroidUtils::AndroidUtils(QObject *parent)
    : QObject(parent)
{
}

AndroidUtils &AndroidUtils::instance()
{
    static AndroidUtils _instance;
    return _instance;
}

void AndroidUtils::checkPendingIntents()
{
    QJniObject activity = QJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    Q_ASSERT(activity.isValid());

    QJniObject::callStaticMethod<void>("org/kde/tokodon/OpenUriActivity", "checkPendingIntents");
}

extern "C" {
JNIEXPORT void JNICALL Java_org_kde_tokodon_AndroidUtils_openUri(JNIEnv *env, jobject obj, jstring path)
{
    Q_UNUSED(obj)
    const char *pathStr = env->GetStringUTFChars(path, NULL);

    NetworkController::instance().setAuthCode(QString::fromUtf8(pathStr));
    env->ReleaseStringUTFChars(path, pathStr);
}
}