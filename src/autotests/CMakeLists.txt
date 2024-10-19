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

ecm_add_test(blurhashtest.cpp
    TEST_NAME blurhashtest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
    )

ecm_add_test(posteditortest.cpp
    TEST_NAME posteditortest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
    )

ecm_add_test(federationtooltest.cpp
    TEST_NAME federationtooltest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(iprulestooltest.cpp
    TEST_NAME iprulestooltest
    LINK_LIBRARIES tokodon_test_static Qt::Test
    NAME_PREFIX "tokodon-"
)

ecm_add_test(emailblocktooltest.cpp
	TEST_NAME emailblocktooltest
	LINK_LIBRARIES tokodon_test_static Qt::Test
	NAME_PREFIX "tokodon-"
)

ecm_add_test(announcementstest.cpp
		TEST_NAME announcementstest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

ecm_add_test(customemojitest.cpp
	TEST_NAME customemojitest
	LINK_LIBRARIES tokodon_test_static Qt::Test
	NAME_PREFIX "tokodon-"
)

ecm_add_test(rulesmodeltest.cpp
		TEST_NAME rulesmodeltest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

ecm_add_test(listsmodeltest.cpp
		TEST_NAME listsmodeltest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

ecm_add_test(polltimemodeltest.cpp
		TEST_NAME polltimemodeltest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

ecm_add_test(socialgraphmodeltest.cpp
		TEST_NAME socialgraphmodeltest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

ecm_add_test(texthandlertest.cpp
		TEST_NAME texthandlertest
		LINK_LIBRARIES tokodon_test_static Qt::Test
		NAME_PREFIX "tokodon-"
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux" AND NOT "$ENV{KDECI_BUILD}" STREQUAL "TRUE")
	add_subdirectory(appiumtests)
endif()