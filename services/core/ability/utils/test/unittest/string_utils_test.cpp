/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "string_utils.h"

using namespace testing::ext;

namespace OHOS {
namespace UpdateEngine {
class StringUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() final {};
    void TearDown() final {};
};

/**
 * @tc.name: LTrimWithLeftSpace
 * @tc.desc: Trim with left space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, LTrimWithLeftSpace, TestSize.Level1)
{
    std::string sourceStr = " testStr";
    StringUtils::LTrim(sourceStr);
    EXPECT_EQ("testStr", sourceStr);
}

/**
 * @tc.name: LTrimWithoutLeftSpace
 * @tc.desc: Trim without left space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, LTrimWithoutLeftSpace, TestSize.Level1)
{
    std::string sourceStr = "testStr ";
    StringUtils::LTrim(sourceStr);
    EXPECT_EQ("testStr ", sourceStr);
}

/**
 * @tc.name: RTrimWithRightSpace
 * @tc.desc: Trim with right space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, RTrimWithRightSpace, TestSize.Level1)
{
    std::string sourceStr = "testStr ";
    StringUtils::RTrim(sourceStr);
    EXPECT_EQ("testStr", sourceStr);
}

/**
 * @tc.name: RTrimWithoutRightSpace
 * @tc.desc: Trim without right space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, RTrimWithoutRightSpace, TestSize.Level1)
{
    std::string sourceStr = " testStr";
    StringUtils::RTrim(sourceStr);
    EXPECT_EQ(" testStr", sourceStr);
}

/**
 * @tc.name: TrimWithSpace
 * @tc.desc: Trim with left and right space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, TrimWithSpace, TestSize.Level1)
{
    std::string sourceStr = "  testStr  ";
    StringUtils::Trim(sourceStr);
    EXPECT_EQ("testStr", sourceStr);
}

/**
 * @tc.name: TrimWithLeftSpace
 * @tc.desc: Trim with left space.
 * @tc.type: FUNC
 * @tc.require: Source string
 */
HWTEST_F(StringUtilsTest, TrimWithLeftSpace, TestSize.Level1)
{
    std::string sourceStr = "  testStr";
    StringUtils::Trim(sourceStr);
    EXPECT_EQ("testStr", sourceStr);
}

/**
 * @tc.name: SplitWhenDelimiterFound
 * @tc.desc: Split str when delimiter found.
 * @tc.type: FUNC
 * @tc.require: Source string and delimiter
 */
HWTEST_F(StringUtilsTest, SplitWhenDelimiterFound, TestSize.Level1)
{
    std::string sourceStr = "10.105.111.23";
    auto splits = StringUtils::Split(sourceStr, '.');
    std::vector<std::string> expect = {"10", "105", "111", "23"};
    EXPECT_EQ(expect, splits);
}

/**
 * @tc.name: SplitWhenDelimiterNotFound
 * @tc.desc: Split str when delimiter not found.
 * @tc.type: FUNC
 * @tc.require: Source string and delimiter
 */
HWTEST_F(StringUtilsTest, SplitWhenDelimiterNotFound, TestSize.Level1)
{
    std::string sourceStr = "10.105.111.23";
    auto splits = StringUtils::Split(sourceStr, ',');
    std::vector<std::string> expect = {"10.105.111.23"};
    EXPECT_EQ(expect, splits);
}

/**
 * @tc.name: GetTrueBoolStr
 * @tc.desc: Get bool str for true.
 * @tc.type: FUNC
 * @tc.require: Bool value
 */
HWTEST_F(StringUtilsTest, GetTrueBoolStr, TestSize.Level1)
{
    EXPECT_EQ("true", StringUtils::GetBoolStr(true));
}

/**
 * @tc.name: GetFalseBoolStr
 * @tc.desc: Get bool str for false.
 * @tc.type: FUNC
 * @tc.require: Bool value
 */
HWTEST_F(StringUtilsTest, GetFalseBoolStr, TestSize.Level1)
{
    EXPECT_EQ("false", StringUtils::GetBoolStr(false));
}

/**
 * @tc.name: SafeSubStringEmpty
 * @tc.desc: source string is empty
 * @tc.type: FUNC
 * @tc.require: source string && beginIndex && endIndex && default string
 */
HWTEST_F(StringUtilsTest, SafeSubStringEmpty, TestSize.Level1)
{
    std::string sourceString = "";
    std::string defaultString = "default";
    int beginIndex = 1;
    int endIndex = 5;
    EXPECT_EQ("default", StringUtils::SafeSubString(sourceString, beginIndex, endIndex, defaultString));
}

/**
 * @tc.name: SafeSubStringBeginLessThanZero
 * @tc.desc: beginIndex is less than 0
 * @tc.type: FUNC
 * @tc.require: source string && beginIndex && endIndex && default string
 */
HWTEST_F(StringUtilsTest, SafeSubStringBeginLessThanZero, TestSize.Level1)
{
    std::string sourceString = "safeSubString Testing";
    std::string defaultString = "default";
    int beginIndex = -1;
    int endIndex = 5;
    EXPECT_EQ("default", StringUtils::SafeSubString(sourceString, beginIndex, endIndex, defaultString));
}

/**
 * @tc.name: SafeSubStringEndBigThanSize
 * @tc.desc: endIndex is big than sourcestring's size
 * @tc.type: FUNC
 * @tc.require: source string && beginIndex && endIndex && default string
 */
HWTEST_F(StringUtilsTest, SafeSubStringEndBigThanSize, TestSize.Level1)
{
    std::string sourceString = "safeSubString Testing";
    std::string defaultString = "default";
    int beginIndex = 1;
    int endIndex = 40;
    EXPECT_EQ("default", StringUtils::SafeSubString(sourceString, beginIndex, endIndex, defaultString));
}

/**
 * @tc.name: SafeSubStringBeginBigThanEnd
 * @tc.desc: beginIndex is big than endIndex
 * @tc.type: FUNC
 * @tc.require: source string && beginIndex && endIndex && default string
 */
HWTEST_F(StringUtilsTest, SafeSubStringBeginBigThanEnd, TestSize.Level1)
{
    std::string sourceString = "safeSubString Testing";
    std::string defaultString = "default";
    int beginIndex = 10;
    int endIndex = 5;
    EXPECT_EQ("default", StringUtils::SafeSubString(sourceString, beginIndex, endIndex, defaultString));
}

/**
 * @tc.name: SafeSubStringNormal
 * @tc.desc: all parameters is ok
 * @tc.type: FUNC
 * @tc.require: source string && beginIndex && endIndex && default string
 */
HWTEST_F(StringUtilsTest, SafeSubStringNormal, TestSize.Level1)
{
    std::string sourceString = "safeSubString Testing";
    std::string defaultString = "default";
    int beginIndex = 5;
    int endIndex = 9;
    EXPECT_NE("default", StringUtils::SafeSubString(sourceString, beginIndex, endIndex, defaultString));
}

/**
 * @tc.name: GetEnumValueStringWithoutSplit
 * @tc.desc: EnumValueString Without Split
 * @tc.type: FUNC
 * @tc.require: enumItem String
 */
HWTEST_F(StringUtilsTest, GetEnumValueStringWithoutSplit, TestSize.Level1)
{
    std::string itemValueString = "enumString";
    EXPECT_EQ("", StringUtils::GetEnumValueString(itemValueString));
}


/**
 * @tc.name: GetEnumValueStringWithSplit
 * @tc.desc: EnumValueString With Split
 * @tc.type: FUNC
 * @tc.require: enumItem String
 */
HWTEST_F(StringUtilsTest, GetEnumValueStringWithSplit, TestSize.Level1)
{
    std::string itemValueString = "enumString::GetTaskInfo";
    EXPECT_EQ("GetTaskInfo", StringUtils::GetEnumValueString(itemValueString));
}

/**
 * @tc.name: GetLastSplitStringFind
 * @tc.desc: find last split string
 * @tc.type: FUNC
 * @tc.require: stringName && splitStr
 */
HWTEST_F(StringUtilsTest, GetLastSplitStringFind, TestSize.Level1)
{
    std::string splitString = "abc,def,ghi";
    std::string splitStr = ",";
    EXPECT_EQ("ghi", StringUtils::GetLastSplitString(splitString, splitStr));
}

/**
 * @tc.name: GetLastSplitStringNotFind
 * @tc.desc: not find split string
 * @tc.type: FUNC
 * @tc.require: stringName && splitStr
 */
HWTEST_F(StringUtilsTest, GetLastSplitStringNotFind, TestSize.Level1)
{
    std::string splitString = "abc,def,ghi";
    std::string splitStr = ":";
    EXPECT_EQ("", StringUtils::GetLastSplitString(splitString, splitStr));
}

/**
 * @tc.name: ReplaceStringAllFind
 * @tc.desc: find substring
 * @tc.type: FUNC
 * @tc.require: srcString && subString && newString
 */
HWTEST_F(StringUtilsTest, ReplaceStringAllFind, TestSize.Level1)
{
    std::string sourceString = "abc###def";
    std::string subString = "###";
    std::string newString = "Vol";
    StringUtils::ReplaceStringAll(sourceString, subString, newString);
    EXPECT_EQ("abcVoldef", sourceString);
}

/**
 * @tc.name: ReplaceStringAllFindMulti
 * @tc.desc: find substring multi
 * @tc.type: FUNC
 * @tc.require: srcString && subString && newString
 */
HWTEST_F(StringUtilsTest, ReplaceStringAllFindMulti, TestSize.Level1)
{
    std::string sourceString = "abc###def###ghi###";
    std::string subString = "###";
    std::string newString = "Vol";
    StringUtils::ReplaceStringAll(sourceString, subString, newString);
    EXPECT_EQ("abcVoldefVolghiVol", sourceString);
}

/**
 * @tc.name: ReplaceStringAllNotFind
 * @tc.desc: not find substring
 * @tc.type: FUNC
 * @tc.require: srcString && subString && newString
 */
HWTEST_F(StringUtilsTest, ReplaceStringAllNotFind, TestSize.Level1)
{
    std::string sourceString = "abc###def###ghi###";
    std::string subString = ",";
    std::string newString = "Vol";
    StringUtils::ReplaceStringAll(sourceString, subString, newString);
    EXPECT_EQ("abc###def###ghi###", sourceString);
}

/**
 * @tc.name: StringRemoveStartNotFind
 * @tc.desc: start string is not find
 * @tc.type: FUNC
 * @tc.require: source string && start string && end string
 */
HWTEST_F(StringUtilsTest, StringRemoveStartNotFind, TestSize.Level1)
{
    std::string sourceString = "<language-zh>zh-cn</language-zh><language-en>en-us</language-en><item>xxxx</item>";
    std::string startString = "<language-gdb>";
    std::string endString = "</language-gdb>";
    StringUtils::StringRemove(sourceString, startString, endString);
    EXPECT_EQ("<language-zh>zh-cn</language-zh><language-en>en-us</language-en><item>xxxx</item>", sourceString);
}

/**
 * @tc.name: StringRemoveEndNotFind
 * @tc.desc: end string is not find
 * @tc.type: FUNC
 * @tc.require: source string && start string && end string
 */
HWTEST_F(StringUtilsTest, StringRemoveEndNotFind, TestSize.Level1)
{
    std::string sourceString = "<language-zh>zh-cn</language-zh><language-en>en-us</language-en><item>xxxx</item>";
    std::string startString = "<language-zh>";
    std::string endString = "</language-gdb>";
    StringUtils::StringRemove(sourceString, startString, endString);
    EXPECT_EQ("<language-zh>zh-cn</language-zh><language-en>en-us</language-en><item>xxxx</item>", sourceString);
}

/**
 * @tc.name: StringRemoveNormal
 * @tc.desc: start and end string find
 * @tc.type: FUNC
 * @tc.require: source string && start string && end string
 */
HWTEST_F(StringUtilsTest, StringRemoveNormal, TestSize.Level1)
{
    std::string sourceString = "<language-zh>zh-cn</language-zh><language-en>en-us</language-en><item>xxxx</item>";
    std::string startString = "<language-zh>";
    std::string endString = "</language-zh>";
    StringUtils::StringRemove(sourceString, startString, endString);
    EXPECT_EQ("<language-en>en-us</language-en><item>xxxx</item>", sourceString);
}
} // namespace UpdateEngine
} // namespace OHOS