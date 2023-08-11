// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-2.0-or-later OR GPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "androidutils.h"

#include <jni.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#else
#include <QCoreApplication>
#include <QJniObject>
#endif

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QAndroidJniObject activity =
        QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;"); // activity is valid
    Q_ASSERT(activity.isValid());

    QAndroidJniEnvironment _env;
    QAndroidJniObject::callStaticMethod<void>("org/kde/tokodon/OpenUriActivity", "checkPendingIntents");
    if (_env->ExceptionCheck()) {
        _env->ExceptionClear();
        qWarning() << "couldn't launch intent";
    }
#else
    QJniObject activity = QJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    Q_ASSERT(activity.isValid());

    QJniObject::callStaticMethod<void>("org/kde/tokodon/OpenUriActivity", "checkPendingIntents");
#endif
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