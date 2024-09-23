#!/usr/bin/env python3

# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2023 Rishi Kumar <rsi.dev17@gmail.com>

import sys
import unittest
import time
from appium import webdriver
from appium.options.common.base import AppiumOptions
from appium.webdriver.common.appiumby import AppiumBy
from appium.options.common.app_option import AppOption
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC


class ATSPIOptions(AppiumOptions, AppOption):
    pass


class TimelineTest(unittest.TestCase):

    def setUp(self):
        options = ATSPIOptions()
        options.app = tokodon_offline_path
        options.set_capability("timeouts", {'implicit': 10000})
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            options=options)

    def tearDown(self):
        if not self._outcome.result.wasSuccessful():
            self.driver.get_screenshot_as_file("failed_test_shot_{}.png".format(self.id()))

    def test_status_type(self):
        self.assertTrue(self.driver.find_element(by='description', value="Normal Status"))
        self.assertTrue(self.driver.find_element(by='description', value="Spoiler Status"))

    def test_favourite_interactions(self):
        favouriteButton=self.driver.find_element(by='description',value="Favorite")
        favouriteButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Favorited"))

    def test_bookmark_interactions(self):
        bookmarkButton=self.driver.find_element(by='description',value="Bookmark")
        bookmarkButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Bookmarked"))

    def test_boost_interactions(self):
        boostButton=self.driver.find_element(by='description',value="Boost")
        boostButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Boosted"))

    def test_status_media(self):
        timelineElement = self.driver.find_element(AppiumBy.CLASS_NAME, "[page tab | Home]")
        timelineElement.send_keys(Keys.DOWN)
        timelineElement.send_keys(Keys.DOWN)
        time.sleep(2)

        self.assertTrue(self.driver.find_element(by='description', value="Status with image attachment"))

        timelineElement.send_keys(Keys.DOWN)
        timelineElement.send_keys(Keys.DOWN)
        time.sleep(2)

        self.assertTrue(self.driver.find_element(by='description', value="Status with Video attachment"))
        self.assertTrue(self.driver.find_element(by='description', value="Status with GifV attachment"))


if __name__ == '__main__':
    tokodon_offline_path = sys.argv[1]
    sys.argv.pop()
    unittest.main()