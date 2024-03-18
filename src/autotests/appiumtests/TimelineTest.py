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
        self.driver.get_screenshot_as_file("failed_test_shot_{}.png".format(self.id()))
        self.driver.quit()

    def test_status_media(self):
        timelineElement = self.driver.find_element(by='name',value="Home Timeline")
        timelineElement.send_keys(Keys.DOWN)
        timelineElement.send_keys(Keys.DOWN)
        timelineElement.send_keys(Keys.DOWN)
        self.assertTrue(self.driver.find_element(by='description', value="Status with image attachment"))
        self.assertTrue(self.driver.find_element(by='description', value="Status with Video attachment"))
        self.assertTrue(self.driver.find_element(by='description', value="Status with GifV attachment"))


if __name__ == '__main__':
    tokodon_offline_path = sys.argv[1]
    sys.argv.pop()
    unittest.main()