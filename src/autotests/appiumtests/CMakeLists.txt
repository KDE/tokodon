# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022-2023 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>

find_package(SeleniumWebDriverATSPI)
set_package_properties(SeleniumWebDriverATSPI PROPERTIES
        DESCRIPTION "Server component for selenium tests using Linux accessibility infrastructure"
        PURPOSE "Automatic GUI testing"
        URL "https://invent.kde.org/sdk/selenium-webdriver-at-spi"
        TYPE OPTIONAL
        )
if(NOT SeleniumWebDriverATSPI_FOUND)
    return()
endif()

find_program(SELENIUM_RUN selenium-webdriver-at-spi-run)

add_test(
        NAME SearchboxTest
        COMMAND ${SELENIUM_RUN} ${CMAKE_CURRENT_SOURCE_DIR}/SearchboxTest.py $<TARGET_FILE:tokodon-offline>
)
add_test(
        NAME TimelineTest
        COMMAND ${SELENIUM_RUN} ${CMAKE_CURRENT_SOURCE_DIR}/TimelineTest.py $<TARGET_FILE:tokodon-offline>
)
set_tests_properties(SearchboxTest PROPERTIES TIMEOUT 300 ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0;USE_CUSTOM_BUS=1;GDK_BACKEND=x11")
set_tests_properties(TimelineTest PROPERTIES TIMEOUT 300 ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0;USE_CUSTOM_BUS=1;GDK_BACKEND=x11")
