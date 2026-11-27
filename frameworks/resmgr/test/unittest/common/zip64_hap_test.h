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

#ifndef RESMGR_ZIP64_HAP_TEST_H
#define RESMGR_ZIP64_HAP_TEST_H

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "resource_manager.h"

namespace OHOS {
namespace Global {
namespace Resource {
namespace Zip64Test {

inline const std::string ZIP64_DIR = "/data/test/zip64/";
constexpr mode_t DIR_MODE = 0700;
constexpr uint64_t ONE_MIB = 1024ULL * 1024;
constexpr uint64_t ONE_GIB = 1024ULL * 1024 * 1024;
constexpr uint64_t FOUR_GB = 4 * ONE_GIB;
// Sparse data area starts beyond 4GB so that every produced entry offset exceeds 32 bits.
constexpr uint64_t BASE_OFFSET = FOUR_GB + ONE_MIB;
// Classic (non-zip64) archive whose data area starts above INT32_MAX but below 4GiB. On 32-bit
// platforms the old `static_cast<long>` truncated such offsets; int64_t keeps them intact.
constexpr uint64_t CLASSIC_BASE_OFFSET = 3 * ONE_GIB;
constexpr uint32_t ZIP_LOCAL_SIG = 0x04034b50;
constexpr uint32_t ZIP_CENTRAL_SIG = 0x02014b50;
constexpr uint32_t ZIP_EOCD_SIG = 0x06054b50;
constexpr uint32_t ZIP64_EOCD_SIG = 0x06064b50;
constexpr uint32_t ZIP64_LOCATOR_SIG = 0x07064b50;
constexpr uint16_t ZIP64_EXTRA_ID = 0x0001;
constexpr uint32_t U32_MAX_VALUE = 0xFFFFFFFF;
constexpr uint16_t ZIP_VERSION = 45;
constexpr uint16_t DOS_DATE = 0x21;
constexpr uint64_t ZIP64_EOCD_RECORD_SIZE = 44;
constexpr size_t ZIP64_EOCD_LEN = 56;
constexpr size_t ZIP64_LOCATOR_LEN = 20;
constexpr size_t ZIP_EOCD_LEN = 22;
constexpr uint64_t BAD_ENTRY_OFFSET_DELTA = 4096;
constexpr uint32_t DIVIDER_COLOR_VALUE = 0x10000000;
// color_aboutPage_title_primary; the same value is asserted for the V2 test data by the native suite.
constexpr uint32_t ABOUT_PAGE_TITLE_COLOR = 0xFF191919;
constexpr size_t TEST_RAWFILE_LEN = 17;
constexpr size_t RAWFILE_BUFFER_LEN = TEST_RAWFILE_LEN + 1;  // include the trailing NUL
// Bundle manager caps packages above 4GB at 200GB (spec BR-8).
constexpr uint64_t MAX_SUPPORTED_HAP_SIZE = 200 * ONE_GIB;
inline constexpr const char *TEST_RAWFILE_CONTENT = "for raw file test";
inline constexpr const char *PATCHED_RAWFILE_CONTENT = "patched raw file!";

// Result of loading a >4GB archive; distinguishes a missing container capability from a broken
// test archive so that the suite can skip (not fail) when ability_base has no zip64 support.
enum class LargeHapLoadResult {
    LOADED,
    UNSUPPORTED,
    BROKEN,
};

struct Zip64Entry {
    std::string name;
    std::vector<uint8_t> data;
    uint64_t headerOffset = 0;
};

// Creates the private test directory; called by every archive builder so suite order does not matter.
void EnsureZip64Dir();

// Safety net for ASSERT_* early returns: removes every archive left in the test directory.
void CleanupAllTestArchives();

void CleanupTestFile(const std::string &path);

// >4GB archives require a 64-bit size_t: ZipFileReader::GetFileLen rejects file sizes above
// SIZE_MAX, so on 32-bit platforms such archives must be rejected instead of loaded.
bool SupportsLargeHap();

bool ReadWholeFile(const std::string &path, std::vector<uint8_t> &out);

std::string TestDataPath(const std::string &relative);

bool WriteAt(FILE *fp, uint64_t offset, const void *data, size_t len);

// Builds a stored (uncompressed) zip64 archive whose first entry starts at BASE_OFFSET (> 4GiB).
// The central directory keeps 32-bit fields where possible and moves the local header offset into
// the zip64 extra field, matching the layout understood by AbilityBase::ZipFile.
bool BuildZip64Hap(const std::string &hapPath, std::vector<Zip64Entry> &entries, bool corruptLocator,
    const std::string &badEntryName, uint64_t badEntryOffset, bool useZip64 = true);

// icon.png exists in several density folders of the test index; the matcher picks one based on the
// device density, so the synthetic archive carries the same bytes in every non-locale density path.
const std::vector<std::string> &MediaDensityPaths();

bool LoadTestData(const std::string &relative, std::vector<uint8_t> &out);

bool BuildBigHap(const std::string &hapPath, bool corruptLocator = false,
    const std::string &badEntryName = "", uint64_t badEntryOffset = 0, bool useV2Index = false);

// Builds a classic (non-zip64) archive with data above INT32_MAX but below 4GiB.
bool BuildClassicHap(const std::string &hapPath);

bool BuildOverlayHap(const std::string &hapPath);

bool BuildPatchHap(const std::string &hapPath, const std::string &rawfileContent);

bool BuildIndexOnlyHap(const std::string &hapPath);

bool WriteGarbageFile(const std::string &path);

std::shared_ptr<ResourceManager> CreateTestResourceManager();

// Loads a >4GB archive and classifies the failure. A control HAP is loaded to prove that the
// environment works, so a rejected >4GB archive means "container lacks zip64 support" (skip)
// rather than "test archive generation is broken" (fail).
LargeHapLoadResult LoadLargeHap(const std::shared_ptr<ResourceManager> &manager,
    const std::string &hapPath);

} // namespace Zip64Test
} // namespace Resource
} // namespace Global
} // namespace OHOS

#endif // RESMGR_ZIP64_HAP_TEST_H
