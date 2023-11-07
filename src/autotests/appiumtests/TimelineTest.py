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


class TimelineTest(unittest.TestCase):

    def setUp(self):
        options = ATSPIOptions()
        options.app = tokodon_offline_path
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            options=options)

    def tearDown(self):
        if not self._outcome.result.wasSuccessful():
            self.driver.get_screenshot_as_file(f"failed_test_shot_tokodon_#{self.id()}.png")

    def test_status_type(self):
        self.assertTrue(self.driver.find_element(by='description', value="Normal Status"))
        self.assertTrue(self.driver.find_element(by='description', value="Spoiler Status"))

    def test_favourite_interactions(self):
        favouriteButton=self.driver.find_element(by='description',value="Favourite")
        favouriteButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Favourited"))

    def test_bookmark_interactions(self):
        bookmarkButton=self.driver.find_element(by='description',value="Bookmark")
        bookmarkButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Bookmarked"))

    def test_boost_interactions(self):
        boostButton=self.driver.find_element(by='description',value="Boost")
        boostButton.click()
        self.assertTrue(self.driver.find_element(by='description', value="Boosted"))

    def test_status_media(self):
        searchElement = self.driver.find_element(by=AppiumBy.NAME, value="Home")
        searchElement.send_keys(Keys.DOWN)
        searchElement.send_keys(Keys.DOWN)
        searchElement.send_keys(Keys.DOWN)
        self.assertTrue(self.driver.find_element(by='description', value="Status with image attachment"))
        self.assertTrue(self.driver.find_element(by='description', value="Status with Video attachment"))
        self.assertTrue(self.driver.find_element(by='description', value="Status with GifV attachment"))


if __name__ == '__main__':
    tokodon_offline_path = sys.argv[1]
    sys.argv.pop()
    unittest.main()