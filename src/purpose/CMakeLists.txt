# SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
# SPDX-License-Identifier: BSD-2-Clause

kcoreaddons_add_plugin(tokodonplugin SOURCES purposeplugin.cpp INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/purpose")
target_link_libraries(tokodonplugin Qt::DBus KF${QT_MAJOR_VERSION}::Purpose KF${QT_MAJOR_VERSION}::KIOGui)