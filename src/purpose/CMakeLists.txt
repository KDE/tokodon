# SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
# SPDX-License-Identifier: BSD-2-Clause

kcoreaddons_add_plugin(tokodonplugin SOURCES purposeplugin.cpp INSTALL_NAMESPACE "kf6/purpose")
target_link_libraries(tokodonplugin Qt::DBus KF6::Purpose KF6::KIOGui)