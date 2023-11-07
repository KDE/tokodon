#!/usr/bin/env python3

# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>

import sys
import unittest
from appium import webdriver
from appium.options.common.base import AppiumOptions
from appium.webdriver.common.appiumby import AppiumBy
from appium.options.common.app_option import AppOption
from selenium.webdriver.common.keys import Keys


class ATSPIOptions(AppiumOptions, AppOption):
    pass


class SearchBoxTest(unittest.TestCase):

    def setUp(self):
        options = ATSPIOptions()
        options.app = tokodon_offline_path
        options.set_capability("timeouts", {'implicit': 30000})
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            options=options)


    def tearDown(self):
        if not self._outcome.result.wasSuccessful():
            self.driver.get_screenshot_as_file(f"failed_test_shot_tokodon_#{self.id()}.png")


    def test_search_and_app(self):
        searchElement = self.driver.find_element(by=AppiumBy.NAME, value="Search")
        searchFocused = searchElement.get_attribute('focused')
        self.assertTrue(searchFocused)
        searchElement.click()
        searchElement.send_keys("myquery")
        searchElement.send_keys(Keys.ENTER)

        self.assertTrue(self.driver.find_element(by=AppiumBy.NAME, value="Users") or self.driver.find_element(by=AppiumBy.NAME, value="Post") or self.driver.find_element(by=AppiumBy.NAME, value="Hashtags") )



if __name__ == '__main__':
    tokodon_offline_path = sys.argv[1]
    sys.argv.pop()
    unittest.main()