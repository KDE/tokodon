# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(posttest.cpp
    TEST_NAME posttest
    LINK_LIBRARIES tokodon_static Qt::Test
    NAME_PREFIX "tokodon-"
)
