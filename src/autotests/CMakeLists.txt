# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

add_library(tokodon_test_static STATIC mockaccount.cpp)
target_link_libraries(tokodon_test_static PUBLIC tokodon_static)

ecm_add_test(posttest.cpp
    TEST_NAME posttest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(timelinetest.cpp
    TEST_NAME timelinetest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(profileeditortest.cpp
    TEST_NAME profileeditortest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(searchtest.cpp
    TEST_NAME searchtest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(conversationmodeltest.cpp
    TEST_NAME conversationmodeltest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)
