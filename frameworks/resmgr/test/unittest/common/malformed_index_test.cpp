/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <vector>

#include "hap_parser.h"
#include "hap_parser_v1.h"
#include "hap_parser_v2.h"
#include "res_desc.h"
#include "test_common.h"
#include "utils/errors.h"

using namespace OHOS::Global::Resource;
using namespace testing::ext;

namespace {
const std::string MALFORMED_DIR = "/data/test/malformed/";
constexpr mode_t DIR_MODE = 0777;
constexpr uint32_t LANG_VALUE_EN = 0x656E;      // "en" in KeyParam value_ encoding
constexpr uint32_t TEST_RES_ID = 0x01000000;    // minimum application resource id
constexpr uint32_t BAD_OFFSET = 0xFFFF0000;     // offset exceeding any reasonable buffer size
constexpr uint16_t MAX_UINT16 = 0xFFFF;        // maximum uint16 value for overflow testing
constexpr uint32_t MAX_UINT32 = 0xFFFFFFFF;     // maximum uint32 value for overflow testing
constexpr uint8_t NULL_TERMINATOR = 0;          // string null terminator byte
constexpr uint32_t PLACEHOLDER_ZERO = 0;        // placeholder for fields filled later
constexpr uint32_t COUNT_ONE = 1;               // count of one item
constexpr uint32_t COUNT_TWO = 2;               // count of two items
constexpr uint32_t NON_EXIST_RES_ID = 999999;   // non-existent resource id for not-found testing
constexpr size_t SMALL_BUF_SIZE = 4;             // small buffer for strLen overflow test
constexpr size_t TRUNCATED_BUF_SIZE = 1;         // buffer too small for any uint16 field
constexpr uint32_t TEST_DATA_OFFSET = 100;       // test data offset value for ConfigItem
constexpr uint8_t FILLER_BYTE_V1 = 0xAA;
constexpr uint8_t FILLER_BYTE_V2 = 0xBB;
constexpr uint32_t OFFSET_DELTA = 100;           // delta added to buf size to create out-of-bounds offset

// Struct field byte offsets relative to struct start (tag_ is the first field)
// ResKey: tag_[4] + offset_[4] + keyParamsCount_[4]
constexpr size_t RESKEY_KEYPARAMS_COUNT_OFFSET = sizeof(uint32_t) * 2;
// IdsHeader: tag_[4] + length_[4] + typeCount_[4] + idCount_[4]
constexpr size_t IDSHEADER_TYPECOUNT_OFFSET = sizeof(uint32_t) * 2;
constexpr size_t IDSHEADER_IDCOUNT_OFFSET = sizeof(uint32_t) * 3;
// ResItem: resId_[4] + offset_[4] + length_[4]
constexpr size_t RESITEM_LENGTH_OFFSET = sizeof(uint32_t) * 2;
// ResInfo: resId_[4] + length_[4] + valueCount_[4]
constexpr size_t RESINFO_VALUECOUNT_OFFSET = sizeof(uint32_t) * 2;
// TypeInfo: type_[4] + length_[4] + count_[4]
constexpr size_t TYPEINFO_COUNT_OFFSET = sizeof(uint32_t) * 2;
// ResIndexHeader: version_[128] + length_[4] + keyCount_[4] + dataBlockOffset_[4]
constexpr size_t RESINDEX_DATABLOCK_OFFSET_FIELD = sizeof(uint32_t) * 2;

class MalformedIndexTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MalformedIndexTest::SetUpTestCase(void)
{
    g_logLevel = LOG_DEBUG;
    mkdir("/data/test/malformed", DIR_MODE);
}

void MalformedIndexTest::TearDownTestCase(void) {}

void MalformedIndexTest::SetUp(void) {}

void MalformedIndexTest::TearDown(void) {}

bool WriteBufferToFile(const std::string &path, const uint8_t *data, size_t size)
{
    if (data == nullptr && size > 0) {
        return false;
    }
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    if (size > 0) {
        ofs.write(reinterpret_cast<const char *>(data), static_cast<std::streamsize>(size));
    }
    ofs.close();
    return true;
}

bool WriteBufferToFile(const std::string &path, const std::vector<uint8_t> &buf)
{
    return WriteBufferToFile(path, buf.data(), buf.size());
}

void PutU32(std::vector<uint8_t> &buf, uint32_t val)
{
    buf.insert(buf.end(), reinterpret_cast<const uint8_t *>(&val),
        reinterpret_cast<const uint8_t *>(&val) + sizeof(uint32_t));
}

void PutU16(std::vector<uint8_t> &buf, uint16_t val)
{
    buf.insert(buf.end(), reinterpret_cast<const uint8_t *>(&val),
        reinterpret_cast<const uint8_t *>(&val) + sizeof(uint16_t));
}

void PutString(std::vector<uint8_t> &buf, const std::string &str)
{
    buf.insert(buf.end(), str.begin(), str.end());
}

void PutNullTerminator(std::vector<uint8_t> &buf)
{
    buf.push_back(NULL_TERMINATOR);
}

void WriteU32(std::vector<uint8_t> &buf, size_t offset, uint32_t val)
{
    std::copy_n(reinterpret_cast<const uint8_t *>(&val), sizeof(uint32_t), buf.begin() + offset);
}

void WriteU16(std::vector<uint8_t> &buf, size_t offset, uint16_t val)
{
    std::copy_n(reinterpret_cast<const uint8_t *>(&val), sizeof(uint16_t), buf.begin() + offset);
}

void WriteBytes(std::vector<uint8_t> &buf, size_t offset, const char *src, size_t len)
{
    std::copy_n(src, len, buf.begin() + offset);
}

void CopyVersionToBuf(std::vector<uint8_t> &buf, const std::string &version)
{
    std::copy_n(version.begin(), version.size(), buf.begin());
}

std::vector<uint8_t> BuildValidV1Index()
{
    std::vector<uint8_t> buf;
    buf.resize(RES_VERSION_LEN, 0);
    CopyVersionToBuf(buf, "Restool 5.0.1.011");
    PutU32(buf, PLACEHOLDER_ZERO);
    PutU32(buf, COUNT_ONE);
    size_t keyOffset = buf.size();
    PutString(buf, "KEYS");
    PutU32(buf, PLACEHOLDER_ZERO);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, static_cast<uint32_t>(KeyType::LANGUAGES));
    PutU32(buf, LANG_VALUE_EN);
    size_t resIdOffset = buf.size();
    WriteU32(buf, keyOffset + sizeof(uint32_t), static_cast<uint32_t>(resIdOffset));
    size_t idItemOffset = resIdOffset + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    PutString(buf, "IDSS");
    PutU32(buf, COUNT_ONE);
    PutU32(buf, TEST_RES_ID);
    PutU32(buf, static_cast<uint32_t>(idItemOffset));
    PutU32(buf, IdItem::HEADER_LEN);
    PutU32(buf, static_cast<uint32_t>(ResType::STRING));
    PutU32(buf, TEST_RES_ID);
    std::string value = "hello";
    PutU16(buf, static_cast<uint16_t>(value.size() + 1));
    PutString(buf, value);
    PutNullTerminator(buf);
    std::string name = "app_name";
    PutU16(buf, static_cast<uint16_t>(name.size() + 1));
    PutString(buf, name);
    PutNullTerminator(buf);
    WriteU32(buf, RES_VERSION_LEN, static_cast<uint32_t>(buf.size()));
    return buf;
}

struct V2Pos {
    size_t idsOffset;
    size_t idsLenPos;
    size_t typeInfoOffset;
    size_t typeLenPos;
    size_t nameLenPos;
    size_t nameStart;
    size_t nameEnd;
};

void AppendV2HeaderAndKeys(std::vector<uint8_t> &buf)
{
    buf.resize(RES_VERSION_LEN, 0);
    CopyVersionToBuf(buf, "RestoolV2 6.0.0.001");
    PutU32(buf, PLACEHOLDER_ZERO);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, PLACEHOLDER_ZERO);
    PutString(buf, "KEYS");
    PutU32(buf, COUNT_ONE);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, static_cast<uint32_t>(KeyType::LANGUAGES));
    PutU32(buf, LANG_VALUE_EN);
}

void AppendV2IdsAndItems(std::vector<uint8_t> &buf, const std::string &name, uint32_t resId, V2Pos &pos)
{
    pos.idsOffset = buf.size();
    PutString(buf, "IDSS");
    pos.idsLenPos = buf.size();
    PutU32(buf, PLACEHOLDER_ZERO);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, COUNT_ONE);
    pos.typeInfoOffset = buf.size();
    PutU32(buf, static_cast<uint32_t>(ResType::STRING));
    pos.typeLenPos = buf.size();
    PutU32(buf, PLACEHOLDER_ZERO);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, resId);
    PutU32(buf, PLACEHOLDER_ZERO);
    pos.nameLenPos = buf.size();
    PutU32(buf, PLACEHOLDER_ZERO);
    pos.nameStart = buf.size();
    PutString(buf, name);
    pos.nameEnd = buf.size();
}

void FillV2Lengths(std::vector<uint8_t> &buf, const V2Pos &pos)
{
    WriteU32(buf, pos.typeLenPos, static_cast<uint32_t>(pos.nameEnd - pos.typeInfoOffset));
    WriteU32(buf, pos.nameLenPos, static_cast<uint32_t>(pos.nameEnd - pos.nameStart));
    size_t dataBlockOffset = buf.size();
    WriteU32(buf, pos.idsLenPos, static_cast<uint32_t>(dataBlockOffset - pos.idsOffset));
    WriteU32(buf, RES_VERSION_LEN + RESINDEX_DATABLOCK_OFFSET_FIELD, static_cast<uint32_t>(dataBlockOffset));
    WriteU32(buf, RES_VERSION_LEN, static_cast<uint32_t>(buf.size()));
}

std::vector<uint8_t> BuildValidV2Index()
{
    std::vector<uint8_t> buf;
    V2Pos pos;
    AppendV2HeaderAndKeys(buf);
    AppendV2IdsAndItems(buf, "app_name", TEST_RES_ID, pos);
    FillV2Lengths(buf, pos);
    return buf;
}

void SetV1Buffer(HapParserV1 &parser, const std::vector<uint8_t> &buf)
{
    parser.bufLen_ = buf.size();
    parser.buffer_ = std::make_unique<uint8_t[]>(buf.size());
    if (!buf.empty()) {
        std::copy(buf.begin(), buf.end(), parser.buffer_.get());
    }
}

int32_t ParseV1Index(const std::vector<uint8_t> &buf)
{
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV1 parser(rc, SELECT_ALL, true);
    SetV1Buffer(parser, buf);
    return parser.ParseResHex();
}

int32_t ParseV2IndexFromBuffer(const std::vector<uint8_t> &buf)
{
    HapParserV2 parser;
    parser.mMapFile_ = std::make_shared<MmapFile>();
    parser.mMapFile_->mmapLen_ = buf.size();
    if (!buf.empty()) {
        parser.mMapFile_->mmap_ = const_cast<uint8_t *>(buf.data());
    }
    int32_t ret = parser.ParseResHex();
    parser.mMapFile_->mmap_ = nullptr;
    return ret;
}

bool ParseV2IndexFromFile(const std::string &fileName, const std::vector<uint8_t> &buf)
{
    std::string path = MALFORMED_DIR + fileName;
    if (!WriteBufferToFile(path, buf)) {
        return false;
    }
    HapParserV2 parser;
    return parser.Init(path.c_str());
}

// ==================== V1 Parser Tests ====================

/*
 * @tc.name: MalformedV1IndexEmptyFileTest001
 * @tc.desc: Test V1 parser with empty buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexEmptyFileTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf;
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexTruncatedHeaderTest001
 * @tc.desc: Test V1 parser with buffer smaller than ResHeader
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexTruncatedHeaderTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ResHeader::RES_HEADER_LEN - 1, FILLER_BYTE_V1);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexKeyCountZeroTest001
 * @tc.desc: Test V1 parser with keyCount = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexKeyCountZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    uint32_t zero = 0;
    WriteU32(buf, RES_VERSION_LEN + sizeof(uint32_t), zero);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, UNKNOWN_ERROR);
}

/*
 * @tc.name: MalformedV1IndexKeyCountTooLargeTest001
 * @tc.desc: Test V1 parser with keyCount > MAX_RES_KEY_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexKeyCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    uint32_t hugeCount = MAX_RES_KEY_COUNT + 1;
    WriteU32(buf, RES_VERSION_LEN + sizeof(uint32_t), hugeCount);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, UNKNOWN_ERROR);
}

/*
 * @tc.name: MalformedV1IndexLengthZeroTest001
 * @tc.desc: Test V1 parser with length = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexLengthZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    uint32_t zero = 0;
    WriteU32(buf, RES_VERSION_LEN, zero);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, UNKNOWN_ERROR);
}

/*
 * @tc.name: MalformedV1IndexInvalidKeyTypeTest001
 * @tc.desc: Test V1 parser with keyParam type >= KEY_TYPE_MAX
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexInvalidKeyTypeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // KeyParam starts at offset 148 (136 header + 12 ResKey)
    uint32_t invalidType = static_cast<uint32_t>(KeyType::KEY_TYPE_MAX);
    WriteU32(buf, ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN, invalidType);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexKeyParamsCountTooLargeTest001
 * @tc.desc: Test V1 parser with keyParamsCount > MAX_KEY_PARAMS_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexKeyParamsCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    uint32_t hugeCount = MAX_KEY_PARAMS_COUNT + 1;
    WriteU32(buf, ResHeader::RES_HEADER_LEN + RESKEY_KEYPARAMS_COUNT_OFFSET, hugeCount);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexTruncatedKeyParamTest001
 * @tc.desc: Test V1 parser with truncated KeyParam data
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexTruncatedKeyParamTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // Truncate buffer right after ResKey header (before KeyParam data completes)
    buf.resize(ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN + sizeof(uint32_t));
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexInvalidResTypeTest001
 * @tc.desc: Test V1 parser with IdItem resType >= MAX_RES_TYPE
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexInvalidResTypeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // Find IdItem offset: resIdOffset = 136 + 12 + 8 = 156
    // IdParam = 8 bytes, IdItem starts at 156 + 8 = 164
    // resType_ is at offset 4 within IdItem (after size_)
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    uint32_t invalidType = static_cast<uint32_t>(ResType::MAX_RES_TYPE);
    WriteU32(buf, idItemOffset + sizeof(uint32_t), invalidType);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexStringArrayArrLenZeroTest001
 * @tc.desc: Test V1 parser with string array arrLen = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexStringArrayArrLenZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // Set resType to STRINGARRAY to trigger ParseStringArray
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    uint32_t arrayType = static_cast<uint32_t>(ResType::STRINGARRAY);
    WriteU32(buf, idItemOffset + sizeof(uint32_t), arrayType);
    // Set arrLen = 0 at the value position (after IdItem header)
    size_t valueOffset = idItemOffset + IdItem::HEADER_LEN;
    uint16_t arrLen = 0;
    WriteU16(buf, valueOffset, arrLen);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexStringArrayArrLenTooLargeTest001
 * @tc.desc: Test V1 parser with string array arrLen exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexStringArrayArrLenTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    uint32_t arrayType = static_cast<uint32_t>(ResType::STRINGARRAY);
    WriteU32(buf, idItemOffset + sizeof(uint32_t), arrayType);
    size_t valueOffset = idItemOffset + IdItem::HEADER_LEN;
    uint16_t arrLen = MAX_UINT16;
    WriteU16(buf, valueOffset, arrLen);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexStringLenTooLargeTest001
 * @tc.desc: Test V1 parser with string length exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexStringLenTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // IdItem value string strLen at offset idItemOffset + HEADER_LEN
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    size_t valueOffset = idItemOffset + IdItem::HEADER_LEN;
    uint16_t hugeLen = MAX_UINT16;
    WriteU16(buf, valueOffset, hugeLen);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexStringLenZeroTest001
 * @tc.desc: Test V1 parser with string length = 0 for value (includeTemi=true)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexStringLenZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    size_t valueOffset = idItemOffset + IdItem::HEADER_LEN;
    uint16_t zeroLen = 0;
    WriteU16(buf, valueOffset, zeroLen);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexIdCountTooLargeTest001
 * @tc.desc: Test V1 parser with ResId count > MAX_RES_ID_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexIdCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // ResId count is at resIdOffset + sizeof(uint32_t) (after "IDSS" tag)
    size_t resIdOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    uint32_t hugeCount = MAX_RES_ID_COUNT + 1;
    WriteU32(buf, resIdOffset + sizeof(uint32_t), hugeCount);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexTruncatedIdHeaderTest001
 * @tc.desc: Test V1 parser with truncated ResId header
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexTruncatedIdHeaderTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // ResId starts at resIdOffset, truncate before it can be fully read
    size_t resIdOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    buf.resize(resIdOffset + ResId::RESID_HEADER_LEN - 1);
    // Fix ResKey offset to point to the (now truncated) ResId
    uint32_t idOff = static_cast<uint32_t>(resIdOffset);
    WriteU32(buf, ResHeader::RES_HEADER_LEN + sizeof(uint32_t), idOff);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexTruncatedIdItemTest001
 * @tc.desc: Test V1 parser with truncated IdItem data
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexTruncatedIdItemTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    size_t idItemOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + ResId::IDPARAM_HEADER_LEN;
    // Truncate in the middle of IdItem header
    buf.resize(idItemOffset + IdItem::HEADER_LEN - 1);
    // Update length
    uint32_t totalLen = static_cast<uint32_t>(buf.size());
    WriteU32(buf, RES_VERSION_LEN, totalLen);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexValidTest001
 * @tc.desc: Test V1 parser with valid index data (sanity check)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexValidTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, OK);
}

/*
 * @tc.name: MalformedV1IndexFileBasedTest001
 * @tc.desc: Test V1 parser Init with malformed file (empty)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexFileBasedTest001, TestSize.Level1)
{
    std::string path = MALFORMED_DIR + "v1_empty.index";
    ASSERT_TRUE(WriteBufferToFile(path, nullptr, 0));
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV1 parser(rc, SELECT_ALL, true);
    bool ret = parser.Init(path.c_str());
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: MalformedV1IndexFileBasedTest002
 * @tc.desc: Test V1 parser Init with malformed file (truncated)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexFileBasedTest002, TestSize.Level1)
{
    std::vector<uint8_t> buf(10, FILLER_BYTE_V1);
    std::string path = MALFORMED_DIR + "v1_truncated.index";
    ASSERT_TRUE(WriteBufferToFile(path, buf));
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV1 parser(rc, SELECT_ALL, true);
    bool ret = parser.Init(path.c_str());
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: MalformedV1IndexFileBasedTest003
 * @tc.desc: Test V1 parser Init with valid file (sanity check)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexFileBasedTest003, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    std::string path = MALFORMED_DIR + "v1_valid.index";
    ASSERT_TRUE(WriteBufferToFile(path, buf));
    std::shared_ptr<ResConfigImpl> rc = nullptr;
    HapParserV1 parser(rc, SELECT_ALL, true);
    bool ret = parser.Init(path.c_str());
    EXPECT_TRUE(ret);
}

// ==================== V2 Parser Tests ====================

/*
 * @tc.name: MalformedV2IndexEmptyFileTest001
 * @tc.desc: Test V2 parser with empty file
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexEmptyFileTest001, TestSize.Level1)
{
    bool ret = ParseV2IndexFromFile("v2_empty.index", std::vector<uint8_t>());
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: MalformedV2IndexTruncatedHeaderTest001
 * @tc.desc: Test V2 parser with buffer smaller than ResIndexHeader
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTruncatedHeaderTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ResIndexHeader::RES_HEADER_LEN - 1, FILLER_BYTE_V2);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexKeyCountZeroTest001
 * @tc.desc: Test V2 parser with keyCount = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexKeyCountZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    uint32_t zero = 0;
    WriteU32(buf, RES_VERSION_LEN + sizeof(uint32_t), zero);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexKeyCountTooLargeTest001
 * @tc.desc: Test V2 parser with keyCount > MAX_RES_KEY_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexKeyCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    uint32_t hugeCount = MAX_RES_KEY_COUNT + 1;
    WriteU32(buf, RES_VERSION_LEN + sizeof(uint32_t), hugeCount);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexLengthZeroTest001
 * @tc.desc: Test V2 parser with length = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexLengthZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    uint32_t zero = 0;
    WriteU32(buf, RES_VERSION_LEN, zero);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexDataBlockOffsetTooLargeTest001
 * @tc.desc: Test V2 parser with dataBlockOffset > mmapLen
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexDataBlockOffsetTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    uint32_t hugeOffset = static_cast<uint32_t>(buf.size()) + OFFSET_DELTA;
    WriteU32(buf, RES_VERSION_LEN + RESINDEX_DATABLOCK_OFFSET_FIELD, hugeOffset);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexInvalidKeyTypeTest001
 * @tc.desc: Test V2 parser with keyParam type >= KEY_TYPE_MAX
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexInvalidKeyTypeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    // KeyParam starts at offset 140 + 12 (KeyInfo header) = 152
    uint32_t invalidType = static_cast<uint32_t>(KeyType::KEY_TYPE_MAX);
    WriteU32(buf, ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN, invalidType);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexKeyParamsCountTooLargeTest001
 * @tc.desc: Test V2 parser with keyParamsCount > MAX_KEY_PARAMS_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexKeyParamsCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    uint32_t hugeCount = MAX_KEY_PARAMS_COUNT + 1;
    WriteU32(buf, ResIndexHeader::RES_HEADER_LEN + RESKEY_KEYPARAMS_COUNT_OFFSET, hugeCount);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTypeCountTooLargeTest001
 * @tc.desc: Test V2 parser with typeCount > MAX_RES_TYPE_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTypeCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    // IdsHeader starts after header + keys
    // header = 140, KeyInfo = 12, KeyParam = 8 => IdsHeader at 160
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    uint32_t hugeTypeCount = MAX_RES_TYPE_COUNT + 1;
    WriteU32(buf, idsOffset + IDSHEADER_TYPECOUNT_OFFSET, hugeTypeCount);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTypeCountZeroTest001
 * @tc.desc: Test V2 parser with typeCount = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTypeCountZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    uint32_t zero = 0;
    WriteU32(buf, idsOffset + IDSHEADER_TYPECOUNT_OFFSET, zero);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexIdCountTooLargeTest001
 * @tc.desc: Test V2 parser with idCount > MAX_RES_ID_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexIdCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    uint32_t hugeIdCount = MAX_RES_ID_COUNT + 1;
    WriteU32(buf, idsOffset + IDSHEADER_IDCOUNT_OFFSET, hugeIdCount);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexIdCountZeroTest001
 * @tc.desc: Test V2 parser with idCount = 0
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexIdCountZeroTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    uint32_t zero = 0;
    WriteU32(buf, idsOffset + IDSHEADER_IDCOUNT_OFFSET, zero);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTypeInfoTypeTooLargeTest001
 * @tc.desc: Test V2 parser with TypeInfo type >= MAX_RES_TYPE
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTypeInfoTypeTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    uint32_t invalidType = static_cast<uint32_t>(ResType::MAX_RES_TYPE);
    WriteU32(buf, typeInfoOffset, invalidType);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexResItemNameLenTooLargeTest001
 * @tc.desc: Test V2 parser with ResItem name length > UINT16_MAX
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexResItemNameLenTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    size_t resItemOffset = typeInfoOffset + TypeInfo::TYPE_INFO_LEN;
    // ResItem.length_ is at offset 8 within ResItem (after resId_ and offset_)
    uint32_t hugeLen = MAX_UINT16 + 1;
    WriteU32(buf, resItemOffset + RESITEM_LENGTH_OFFSET, hugeLen);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexResItemNameOutOfBoundsTest001
 * @tc.desc: Test V2 parser with ResItem name exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexResItemNameOutOfBoundsTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    size_t resItemOffset = typeInfoOffset + TypeInfo::TYPE_INFO_LEN;
    // Set name length to a value that exceeds buffer but is <= UINT16_MAX
    uint32_t nameLen = 0x1000;
    WriteU32(buf, resItemOffset + RESITEM_LENGTH_OFFSET, nameLen);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTruncatedKeyInfoTest001
 * @tc.desc: Test V2 parser with truncated KeyInfo header
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTruncatedKeyInfoTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    // Truncate after header + partial KeyInfo
    buf.resize(ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN - 1);
    // Fix dataBlockOffset to point within buffer
    uint32_t dataBlock = static_cast<uint32_t>(buf.size());
    WriteU32(buf, RES_VERSION_LEN + RESINDEX_DATABLOCK_OFFSET_FIELD, dataBlock);
    uint32_t totalLen = static_cast<uint32_t>(buf.size());
    WriteU32(buf, RES_VERSION_LEN, totalLen);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexValidTest001
 * @tc.desc: Test V2 parser with valid index data (sanity check)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexValidTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_EQ(ret, OK);
}

// ==================== V2 Static Method Tests ====================

/*
 * @tc.name: MalformedV2ParseStringOutOfBoundsTest001
 * @tc.desc: Test V2 ParseString with strLen exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseStringOutOfBoundsTest001, TestSize.Level1)
{
    uint8_t buf[SMALL_BUF_SIZE] = {0};
    uint16_t strLen = MAX_UINT16;
    std::copy_n(reinterpret_cast<const uint8_t *>(&strLen), sizeof(uint16_t), buf);
    uint32_t offset = 0;
    std::string result;
    int32_t ret = HapParserV2::ParseString(offset, result, sizeof(buf), buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseStringTruncatedHeaderTest001
 * @tc.desc: Test V2 ParseString with buffer too small for length field
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseStringTruncatedHeaderTest001, TestSize.Level1)
{
    uint8_t buf[TRUNCATED_BUF_SIZE] = {0};
    uint32_t offset = 0;
    std::string result;
    int32_t ret = HapParserV2::ParseString(offset, result, sizeof(buf), buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseStringArrayOutOfBoundsTest001
 * @tc.desc: Test V2 ParseStringArray with arrLen exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseStringArrayOutOfBoundsTest001, TestSize.Level1)
{
    uint8_t buf[SMALL_BUF_SIZE] = {0};
    uint16_t arrLen = MAX_UINT16;
    std::copy_n(reinterpret_cast<const uint8_t *>(&arrLen), sizeof(uint16_t), buf);
    uint32_t offset = 0;
    std::vector<std::string> values;
    int32_t ret = HapParserV2::ParseStringArray(offset, values, sizeof(buf), buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseStringArrayTruncatedTest001
 * @tc.desc: Test V2 ParseStringArray with buffer too small for arrLen field
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseStringArrayTruncatedTest001, TestSize.Level1)
{
    uint8_t buf[TRUNCATED_BUF_SIZE] = {0};
    uint32_t offset = 0;
    std::vector<std::string> values;
    int32_t ret = HapParserV2::ParseStringArray(offset, values, sizeof(buf), buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseResInfoValueCountTooLargeTest001
 * @tc.desc: Test V2 ParseResInfo with valueCount > MAX_RES_KEY_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseResInfoValueCountTooLargeTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ResInfo::RES_INFO_LEN, 0);
    // Set valueCount to a huge value at offset 8 within ResInfo
    uint32_t hugeCount = MAX_RES_KEY_COUNT + 1;
    WriteU32(buf, RESINFO_VALUECOUNT_OFFSET, hugeCount);
    uint32_t offset = 0;
    ResInfo resInfo;
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, buf.size(), buf.data());
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseResInfoOutOfBoundsTest001
 * @tc.desc: Test V2 ParseResInfo with buffer too small
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseResInfoOutOfBoundsTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ResInfo::RES_INFO_LEN - 1, 0);
    uint32_t offset = 0;
    ResInfo resInfo;
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, buf.size(), buf.data());
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseResInfoConfigItemsOutOfBoundsTest001
 * @tc.desc: Test V2 ParseResInfo with valueCount * CONFIG_ITEM_LEN exceeding buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseResInfoConfigItemsOutOfBoundsTest001, TestSize.Level1)
{
    // ResInfo (12 bytes) + 1 ConfigItem (8 bytes) = 20 bytes
    // Set valueCount = 2, so 2 * 8 = 16 > 8 available
    std::vector<uint8_t> buf(ResInfo::RES_INFO_LEN + ConfigItem::CONFIG_ITEM_LEN, 0);
    uint32_t valueCount = COUNT_TWO;
    WriteU32(buf, RESINFO_VALUECOUNT_OFFSET, valueCount);
    uint32_t offset = 0;
    ResInfo resInfo;
    int32_t ret = HapParserV2::ParseResInfo(offset, resInfo, buf.size(), buf.data());
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseConfigItemOutOfBoundsTest001
 * @tc.desc: Test V2 ParseConfigItem with buffer too small
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseConfigItemOutOfBoundsTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ConfigItem::CONFIG_ITEM_LEN - 1, 0);
    uint32_t offset = 0;
    ConfigItem configItem;
    int32_t ret = HapParserV2::ParseConfigItem(offset, configItem, buf.size(), buf.data());
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV2ParseConfigItemValidTest001
 * @tc.desc: Test V2 ParseConfigItem with valid data (sanity check)
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2ParseConfigItemValidTest001, TestSize.Level1)
{
    std::vector<uint8_t> buf(ConfigItem::CONFIG_ITEM_LEN, 0);
    uint32_t cfgId = 1;
    uint32_t dataOff = TEST_DATA_OFFSET;
    WriteU32(buf, PLACEHOLDER_ZERO, cfgId);
    WriteU32(buf, sizeof(uint32_t), dataOff);
    uint32_t offset = 0;
    ConfigItem configItem;
    int32_t ret = HapParserV2::ParseConfigItem(offset, configItem, buf.size(), buf.data());
    EXPECT_EQ(ret, OK);
    EXPECT_EQ(offset, static_cast<uint32_t>(ConfigItem::CONFIG_ITEM_LEN));
}

// ==================== Additional Boundary Tests ====================

/*
 * @tc.name: MalformedV1IndexKeyTagMismatchTest001
 * @tc.desc: Test V1 parser with wrong ResKey tag
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexKeyTagMismatchTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // Overwrite "KEYS" tag with invalid bytes
    WriteBytes(buf, ResHeader::RES_HEADER_LEN, "XXXX", sizeof(uint32_t));
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, UNKNOWN_ERROR);
}

/*
 * @tc.name: MalformedV1IndexIdTagMismatchTest001
 * @tc.desc: Test V1 parser with wrong ResId tag
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexIdTagMismatchTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    size_t resIdOffset = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    WriteBytes(buf, resIdOffset, "XXXX", sizeof(uint32_t));
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, UNKNOWN_ERROR);
}

/*
 * @tc.name: MalformedV2IndexKeyTagMismatchTest001
 * @tc.desc: Test V2 parser with wrong KeyInfo tag
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexKeyTagMismatchTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    WriteBytes(buf, ResIndexHeader::RES_HEADER_LEN, "XXXX", sizeof(uint32_t));
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexIdsTagMismatchTest001
 * @tc.desc: Test V2 parser with wrong IdsHeader tag
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexIdsTagMismatchTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    WriteBytes(buf, idsOffset, "XXXX", sizeof(uint32_t));
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexIdsLengthTooLargeTest001
 * @tc.desc: Test V2 parser with IdsHeader length exceeding dataBlockOffset
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexIdsLengthTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    // Set IdsHeader length_ to a value that exceeds dataBlockOffset
    uint32_t hugeLen = MAX_UINT32;
    WriteU32(buf, idsOffset + sizeof(uint32_t), hugeLen);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTypeInfoLengthTooLargeTest001
 * @tc.desc: Test V2 parser with TypeInfo length exceeding dataBlockOffset
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTypeInfoLengthTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    uint32_t hugeLen = MAX_UINT32;
    WriteU32(buf, typeInfoOffset + sizeof(uint32_t), hugeLen);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV2IndexTypeInfoCountTooLargeTest001
 * @tc.desc: Test V2 parser with TypeInfo count > idCount
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV2IndexTypeInfoCountTooLargeTest001, TestSize.Level1)
{
    auto buf = BuildValidV2Index();
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    uint32_t hugeCount = MAX_UINT32;
    WriteU32(buf, typeInfoOffset + TYPEINFO_COUNT_OFFSET, hugeCount);
    int32_t ret = ParseV2IndexFromBuffer(buf);
    EXPECT_NE(ret, OK);
}

/*
 * @tc.name: MalformedV1IndexValueOffsetOutOfBoundsTest001
 * @tc.desc: Test V1 parser with IdParam offset pointing beyond buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexValueOffsetOutOfBoundsTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    // IdParam offset is at resIdOffset + 12 (IDSS tag + count + idParam.id + idParam.offset)
    size_t idParamOffsetPos = ResHeader::RES_HEADER_LEN + ResKey::RESKEY_HEADER_LEN +
        KeyParam::KEYPARAM_LEN + ResId::RESID_HEADER_LEN + sizeof(uint32_t);
    uint32_t hugeOffset = BAD_OFFSET;
    WriteU32(buf, idParamOffsetPos, hugeOffset);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

/*
 * @tc.name: MalformedV1IndexKeyOffsetOutOfBoundsTest001
 * @tc.desc: Test V1 parser with ResKey offset pointing beyond buffer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, MalformedV1IndexKeyOffsetOutOfBoundsTest001, TestSize.Level1)
{
    auto buf = BuildValidV1Index();
    uint32_t hugeOffset = BAD_OFFSET;
    WriteU32(buf, ResHeader::RES_HEADER_LEN + sizeof(uint32_t), hugeOffset);
    int32_t ret = ParseV1Index(buf);
    EXPECT_EQ(ret, SYS_ERROR);
}

// ==================== HapResourceV2 Tests ====================

void AppendV2DataBlock(std::vector<uint8_t> &buf, uint32_t resId, uint32_t cfgId, uint32_t configItemOffset)
{
    PutU32(buf, resId);
    PutU32(buf, ResInfo::RES_INFO_LEN + ConfigItem::CONFIG_ITEM_LEN);
    PutU32(buf, COUNT_ONE);
    PutU32(buf, cfgId);
    PutU32(buf, configItemOffset);
}

std::vector<uint8_t> BuildV2IndexWithDataBlock(const std::string &resName, uint32_t configItemOffset)
{
    const uint32_t resId = TEST_RES_ID;
    const uint32_t cfgId = 1;
    std::vector<uint8_t> buf;
    V2Pos pos;
    AppendV2HeaderAndKeys(buf);
    AppendV2IdsAndItems(buf, resName, resId, pos);
    FillV2Lengths(buf, pos);
    WriteU32(buf, pos.nameLenPos - sizeof(uint32_t), static_cast<uint32_t>(buf.size()));
    AppendV2DataBlock(buf, resId, cfgId, configItemOffset);
    WriteU32(buf, RES_VERSION_LEN, static_cast<uint32_t>(buf.size()));
    return buf;
}

std::shared_ptr<HapResource> LoadV2ResourceFromBuffer(const std::vector<uint8_t> &buf)
{
    HapParserV2 parser;
    parser.mMapFile_ = std::make_shared<MmapFile>();
    parser.mMapFile_->mmapLen_ = buf.size();
    parser.mMapFile_->mmap_ = new uint8_t[buf.size()]();
    if (!buf.empty()) {
        std::copy(buf.begin(), buf.end(), parser.mMapFile_->mmap_);
    }
    parser.mMapFile_->mapper_ = std::make_unique<OHOS::AbilityBase::FileMapper>();
    if (parser.ParseResHex() != OK) {
        return nullptr;
    }
    return parser.GetHapResource("/data/test/index.index", false, false);
}

/*
 * @tc.name: HapResourceV2GetIdItemWithBadOffsetTest001
 * @tc.desc: Test HapResourceV2 GetIdItem returns nullptr with malformed ConfigItem.offset
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2GetIdItemWithBadOffsetTest001, TestSize.Level1)
{
    // Build V2 index with ConfigItem.offset_ pointing beyond buffer
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    // GetIdValues should return non-null (ParseLimitPaths succeeds)
    auto idValues = pResource->GetIdValues(TEST_RES_ID);
    ASSERT_TRUE(idValues != nullptr);
    ASSERT_EQ(idValues->GetLimitPathsConst().size(), static_cast<size_t>(1));

    // GetIdItem should return nullptr (ParseString fails at bad offset)
    auto idItem = idValues->GetLimitPathsConst()[0]->GetIdItem();
    EXPECT_EQ(idItem, nullptr);
}

/*
 * @tc.name: HapResourceV2GetIdItemWithValidDataTest001
 * @tc.desc: Test HapResourceV2 GetIdItem returns valid item with correct data
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2GetIdItemWithValidDataTest001, TestSize.Level1)
{
    // Build V2 index where ConfigItem.offset_ points to valid string data appended after data block
    std::string resName = "app_name";
    auto buf = BuildV2IndexWithDataBlock(resName, 0);
    size_t stringDataOffset = buf.size();
    // Fix ConfigItem.offset_ to point to the appended string data
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    size_t dataBlockOffset = typeInfoOffset + TypeInfo::TYPE_INFO_LEN + ResItem::RES_ITEM_LEN + resName.size();
    size_t configItemOffsetPos = dataBlockOffset + ResInfo::RES_INFO_LEN + sizeof(uint32_t);
    uint32_t strOff = static_cast<uint32_t>(stringDataOffset);
    WriteU32(buf, configItemOffsetPos, strOff);

    // Append string data: strLen(2) + content (V2 format, no '\0')
    std::string value = "hello";
    PutU16(buf, static_cast<uint16_t>(value.size()));
    PutString(buf, value);

    // Fix total length
    uint32_t totalLen = static_cast<uint32_t>(buf.size());
    WriteU32(buf, RES_VERSION_LEN, totalLen);

    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    auto idValues = pResource->GetIdValues(TEST_RES_ID);
    ASSERT_TRUE(idValues != nullptr);
    ASSERT_EQ(idValues->GetLimitPathsConst().size(), static_cast<size_t>(1));

    auto idItem = idValues->GetLimitPathsConst()[0]->GetIdItem();
    ASSERT_TRUE(idItem != nullptr);
    EXPECT_EQ(idItem->value_, "hello");
    EXPECT_EQ(idItem->name_, resName);
}

/*
 * @tc.name: HapResourceV2ParseLimitPathsResIdMismatchTest001
 * @tc.desc: Test HapResourceV2 ParseLimitPaths with mismatched ResInfo.resId
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2ParseLimitPathsResIdMismatchTest001, TestSize.Level1)
{
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    // Tamper ResInfo.resId_ to a different value
    size_t idsOffset = ResIndexHeader::RES_HEADER_LEN + KeyInfo::RESKEY_HEADER_LEN + KeyParam::KEYPARAM_LEN;
    size_t typeInfoOffset = idsOffset + IdsHeader::IDS_HEADER_LEN;
    size_t dataBlockOffset = typeInfoOffset + TypeInfo::TYPE_INFO_LEN + ResItem::RES_ITEM_LEN + strlen("app_name");
    uint32_t badResId = 0xDEAD;
    WriteU32(buf, dataBlockOffset, badResId);

    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    // GetIdValues should return nullptr (ParseLimitPaths fails due to resId mismatch)
    auto idValues = pResource->GetIdValues(TEST_RES_ID);
    EXPECT_EQ(idValues, nullptr);
}

/*
 * @tc.name: HapResourceV2InitThemeSystemResWithBadDataTest001
 * @tc.desc: Test InitThemeSystemRes does not crash with malformed string data
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2InitThemeSystemResWithBadDataTest001, TestSize.Level1)
{
    auto buf = BuildV2IndexWithDataBlock("system_color_change", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);
    EXPECT_FALSE(pResource->isThemeSystemResEnable_);
}

/*
 * @tc.name: HapResourceV2GetIdValuesByNameWithBadDataTest001
 * @tc.desc: Test GetIdValuesByName handles malformed string data gracefully
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2GetIdValuesByNameWithBadDataTest001, TestSize.Level1)
{
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    // GetIdValuesByName should return non-null (ParseLimitPaths succeeds)
    auto idValues = pResource->GetIdValuesByName("app_name", ResType::STRING);
    ASSERT_TRUE(idValues != nullptr);
    ASSERT_EQ(idValues->GetLimitPathsConst().size(), static_cast<size_t>(1));

    // But GetIdItem should return nullptr (string data is malformed)
    auto idItem = idValues->GetLimitPathsConst()[0]->GetIdItem();
    EXPECT_EQ(idItem, nullptr);
}

/*
 * @tc.name: HapResourceV2GetIdValuesNotFoundTest001
 * @tc.desc: Test HapResourceV2 GetIdValues with non-existent id
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2GetIdValuesNotFoundTest001, TestSize.Level1)
{
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    auto idValues = pResource->GetIdValues(NON_EXIST_RES_ID);
    EXPECT_EQ(idValues, nullptr);
}

/*
 * @tc.name: HapResourceV2GetIdValuesByNameNotFoundTest001
 * @tc.desc: Test HapResourceV2 GetIdValuesByName with non-existent name
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2GetIdValuesByNameNotFoundTest001, TestSize.Level1)
{
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);

    auto idValues = pResource->GetIdValuesByName("non_existent", ResType::STRING);
    EXPECT_EQ(idValues, nullptr);
}

// ==================== ParseLimitPaths mmap null check Tests ====================

void TestParseLimitPathsWithNullMmap(bool nullMmapFile, bool nullMmapPtr)
{
    auto buf = BuildV2IndexWithDataBlock("app_name", BAD_OFFSET);
    auto pResource = LoadV2ResourceFromBuffer(buf);
    ASSERT_TRUE(pResource != nullptr);
    auto v2Resource = (HapResourceV2 *)pResource.get();
    auto it = v2Resource->idMap_.find(TEST_RES_ID);
    ASSERT_TRUE(it != v2Resource->idMap_.end());
    auto idValuesV2 = it->second;
    ASSERT_TRUE(idValuesV2 != nullptr);
    ASSERT_FALSE(idValuesV2->IsParsed());
    if (nullMmapFile) {
        idValuesV2->SetMMap(nullptr);
    } else if (nullMmapPtr) {
        auto mmapFile = idValuesV2->GetMMap();
        ASSERT_TRUE(mmapFile != nullptr);
        mmapFile->mmap_ = nullptr;
    }
    int32_t result = v2Resource->ParseLimitPaths(idValuesV2);
    EXPECT_EQ(result, SYS_ERROR);
}

/*
 * @tc.name: HapResourceV2ParseLimitPathsNullMmapFileTest001
 * @tc.desc: Test HapResourceV2 ParseLimitPaths with null mmapFile
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2ParseLimitPathsNullMmapFileTest001, TestSize.Level1)
{
    TestParseLimitPathsWithNullMmap(true, false);
}

/*
 * @tc.name: HapResourceV2ParseLimitPathsNullMmapPtrTest001
 * @tc.desc: Test HapResourceV2 ParseLimitPaths with null mmap_ pointer
 * @tc.type: FUNC
 */
HWTEST_F(MalformedIndexTest, HapResourceV2ParseLimitPathsNullMmapPtrTest001, TestSize.Level1)
{
    TestParseLimitPathsWithNullMmap(false, true);
}
} // namespace
