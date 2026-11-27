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

#include "zip64_hap_test.h"

#include <cerrno>
#include <climits>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#include <zlib.h>

#include "hilog_wrapper.h"
#include "res_config.h"

namespace OHOS {
namespace Global {
namespace Resource {
namespace Zip64Test {
namespace {

constexpr size_t BITS_PER_BYTE = CHAR_BIT;

void PutU16(std::vector<uint8_t> &buf, uint16_t value)
{
    buf.push_back(static_cast<uint8_t>(value & 0xFF));
    buf.push_back(static_cast<uint8_t>((value >> BITS_PER_BYTE) & 0xFF));
}

void PutU32(std::vector<uint8_t> &buf, uint32_t value)
{
    for (size_t index = 0; index < sizeof(uint32_t); ++index) {
        buf.push_back(static_cast<uint8_t>((value >> (index * BITS_PER_BYTE)) & 0xFF));
    }
}

void PutU64(std::vector<uint8_t> &buf, uint64_t value)
{
    for (size_t index = 0; index < sizeof(uint64_t); ++index) {
        buf.push_back(static_cast<uint8_t>((value >> (index * BITS_PER_BYTE)) & 0xFF));
    }
}

void PutBytes(std::vector<uint8_t> &buf, const void *data, size_t len)
{
    if (data == nullptr || len == 0) {
        return;
    }
    const uint8_t *begin = static_cast<const uint8_t *>(data);
    buf.insert(buf.end(), begin, begin + len);
}

uint32_t Crc32Of(const std::vector<uint8_t> &data)
{
    uLong crc = crc32(0L, Z_NULL, 0);
    if (data.empty()) {
        return static_cast<uint32_t>(crc);
    }
    crc = crc32(crc, data.data(), static_cast<uInt>(data.size()));
    return static_cast<uint32_t>(crc);
}

std::vector<uint8_t> MakeLocalHeader(const Zip64Entry &entry, uint32_t crc)
{
    std::vector<uint8_t> header;
    PutU32(header, ZIP_LOCAL_SIG);
    PutU16(header, ZIP_VERSION);
    PutU16(header, 0);              // flags
    PutU16(header, 0);              // compression method: stored
    PutU16(header, 0);              // last mod time
    PutU16(header, DOS_DATE);
    PutU32(header, crc);
    PutU32(header, static_cast<uint32_t>(entry.data.size()));
    PutU32(header, static_cast<uint32_t>(entry.data.size()));
    PutU16(header, static_cast<uint16_t>(entry.name.size()));
    PutU16(header, 0);              // extra size
    PutBytes(header, entry.name.data(), entry.name.size());
    return header;
}

std::vector<uint8_t> MakeCentralEntry(const Zip64Entry &entry, uint32_t crc, uint64_t headerOffset,
    bool useZip64)
{
    std::vector<uint8_t> extra;
    if (useZip64) {
        PutU16(extra, ZIP64_EXTRA_ID);
        PutU16(extra, sizeof(uint64_t));
        PutU64(extra, headerOffset);
    }

    std::vector<uint8_t> header;
    PutU32(header, ZIP_CENTRAL_SIG);
    PutU16(header, ZIP_VERSION);    // version made by
    PutU16(header, ZIP_VERSION);    // version needed
    PutU16(header, 0);              // flags
    PutU16(header, 0);              // compression method
    PutU16(header, 0);              // last mod time
    PutU16(header, DOS_DATE);
    PutU32(header, crc);
    PutU32(header, static_cast<uint32_t>(entry.data.size()));
    PutU32(header, static_cast<uint32_t>(entry.data.size()));
    PutU16(header, static_cast<uint16_t>(entry.name.size()));
    PutU16(header, static_cast<uint16_t>(extra.size()));
    PutU16(header, 0);              // comment size
    PutU16(header, 0);              // disk number start
    PutU16(header, 0);              // internal attributes
    PutU32(header, 0);              // external attributes
    PutU32(header, useZip64 ? U32_MAX_VALUE : static_cast<uint32_t>(headerOffset));
    PutBytes(header, entry.name.data(), entry.name.size());
    PutBytes(header, extra.data(), extra.size());
    return header;
}

std::vector<uint8_t> MakeZip64Eocd(uint64_t count, uint64_t centralSize, uint64_t centralOffset)
{
    std::vector<uint8_t> record;
    PutU32(record, ZIP64_EOCD_SIG);
    PutU64(record, ZIP64_EOCD_RECORD_SIZE);
    PutU16(record, ZIP_VERSION);
    PutU16(record, ZIP_VERSION);
    PutU32(record, 0);              // number of this disk
    PutU32(record, 0);              // disk with central directory start
    PutU64(record, count);
    PutU64(record, count);
    PutU64(record, centralSize);
    PutU64(record, centralOffset);
    return record;
}

std::vector<uint8_t> MakeZip64Locator(uint64_t zip64EocdOffset, bool corruptSignature)
{
    std::vector<uint8_t> locator;
    PutU32(locator, corruptSignature ? 0U : ZIP64_LOCATOR_SIG);
    PutU32(locator, 0);             // disk with zip64 end of central directory
    PutU64(locator, zip64EocdOffset);
    PutU32(locator, 1);             // total number of disks
    return locator;
}

std::vector<uint8_t> MakeEocd(uint16_t count, uint32_t centralSize)
{
    std::vector<uint8_t> record;
    PutU32(record, ZIP_EOCD_SIG);
    PutU16(record, 0);              // number of this disk
    PutU16(record, 0);              // disk with central directory start
    PutU16(record, count);
    PutU16(record, count);
    PutU32(record, centralSize);
    PutU32(record, U32_MAX_VALUE);  // central directory offset placeholder -> zip64 record
    PutU16(record, 0);              // comment length
    return record;
}

std::vector<uint8_t> MakeClassicEocd(uint16_t count, uint32_t centralSize, uint32_t centralOffset)
{
    std::vector<uint8_t> record;
    PutU32(record, ZIP_EOCD_SIG);
    PutU16(record, 0);
    PutU16(record, 0);
    PutU16(record, count);
    PutU16(record, count);
    PutU32(record, centralSize);
    PutU32(record, centralOffset);
    PutU16(record, 0);
    return record;
}

bool WriteEntryData(FILE *fp, std::vector<Zip64Entry> &entries, uint64_t &cursor)
{
    for (auto &entry : entries) {
        entry.headerOffset = cursor;
        std::vector<uint8_t> localHeader = MakeLocalHeader(entry, Crc32Of(entry.data));
        if (!WriteAt(fp, cursor, localHeader.data(), localHeader.size())) {
            return false;
        }
        cursor += localHeader.size();
        if (!WriteAt(fp, cursor, entry.data.data(), entry.data.size())) {
            return false;
        }
        cursor += entry.data.size();
    }
    return true;
}

bool BuildCentralDirectory(const std::vector<Zip64Entry> &entries, bool useZip64,
    const std::string &badEntryName, uint64_t badEntryOffset, std::vector<uint8_t> &central)
{
    for (const auto &entry : entries) {
        uint64_t headerOffset = entry.headerOffset;
        if (!badEntryName.empty() && entry.name == badEntryName) {
            headerOffset = badEntryOffset;
        }
        if (!useZip64 && headerOffset > UINT32_MAX) {
            return false;
        }
        std::vector<uint8_t> item = MakeCentralEntry(entry, Crc32Of(entry.data), headerOffset, useZip64);
        central.insert(central.end(), item.begin(), item.end());
    }
    return true;
}

bool WriteEndRecords(FILE *fp, uint64_t cursor, bool useZip64, bool corruptLocator,
    size_t count, const std::vector<uint8_t> &central, uint64_t centralOffset)
{
    if (!useZip64) {
        std::vector<uint8_t> eocd = MakeClassicEocd(static_cast<uint16_t>(count),
            static_cast<uint32_t>(central.size()), static_cast<uint32_t>(centralOffset));
        return WriteAt(fp, cursor, eocd.data(), eocd.size()) && eocd.size() == ZIP_EOCD_LEN;
    }
    std::vector<uint8_t> zip64Eocd = MakeZip64Eocd(count, central.size(), centralOffset);
    if (!WriteAt(fp, cursor, zip64Eocd.data(), zip64Eocd.size()) ||
        zip64Eocd.size() != ZIP64_EOCD_LEN) {
        return false;
    }
    uint64_t zip64EocdOffset = cursor;
    cursor += zip64Eocd.size();
    std::vector<uint8_t> locator = MakeZip64Locator(zip64EocdOffset, corruptLocator);
    if (!WriteAt(fp, cursor, locator.data(), locator.size()) || locator.size() != ZIP64_LOCATOR_LEN) {
        return false;
    }
    cursor += locator.size();
    std::vector<uint8_t> eocd = MakeEocd(static_cast<uint16_t>(count), static_cast<uint32_t>(central.size()));
    return WriteAt(fp, cursor, eocd.data(), eocd.size()) && eocd.size() == ZIP_EOCD_LEN;
}

} // namespace

void EnsureZip64Dir()
{
    if (mkdir(ZIP64_DIR.c_str(), DIR_MODE) != 0 && errno != EEXIST) {
        RESMGR_HILOGW(RESMGR_TAG, "mkdir %{public}s failed, errno=%{public}d", ZIP64_DIR.c_str(), errno);
        return;
    }
    // The directory may already exist with looser permissions.
    if (chmod(ZIP64_DIR.c_str(), DIR_MODE) != 0) {
        RESMGR_HILOGW(RESMGR_TAG, "chmod %{public}s failed, errno=%{public}d", ZIP64_DIR.c_str(), errno);
    }
}

void CleanupAllTestArchives()
{
    DIR *dir = opendir(ZIP64_DIR.c_str());
    if (dir == nullptr) {
        return;
    }
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        std::string path = ZIP64_DIR + entry->d_name;
        if (remove(path.c_str()) != 0 && errno != ENOENT) {
            RESMGR_HILOGW(RESMGR_TAG, "cleanup %{public}s failed, errno=%{public}d", path.c_str(), errno);
        }
    }
    closedir(dir);
}

void CleanupTestFile(const std::string &path)
{
    if (remove(path.c_str()) != 0 && errno != ENOENT) {
        RESMGR_HILOGW(RESMGR_TAG, "cleanup %{public}s failed, errno=%{public}d", path.c_str(), errno);
    }
}

bool SupportsLargeHap()
{
    return sizeof(size_t) >= sizeof(uint64_t);
}

bool ReadWholeFile(const std::string &path, std::vector<uint8_t> &out)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    in.seekg(0, std::ios::end);
    std::streamoff len = in.tellg();
    if (len < 0) {
        return false;
    }
    in.seekg(0, std::ios::beg);
    out.resize(static_cast<size_t>(len));
    if (len > 0) {
        in.read(reinterpret_cast<char *>(out.data()), len);
    }
    return in.good() || in.eof();
}

std::string TestDataPath(const std::string &relative)
{
    return "/data/test/" + relative;
}

bool WriteAt(FILE *fp, uint64_t offset, const void *data, size_t len)
{
    if (fseeko(fp, static_cast<off_t>(offset), SEEK_SET) != 0) {
        return false;
    }
    if (len == 0) {
        return true;
    }
    return fwrite(data, 1, len, fp) == len;
}

bool BuildZip64Hap(const std::string &hapPath, std::vector<Zip64Entry> &entries, bool corruptLocator,
    const std::string &badEntryName, uint64_t badEntryOffset, bool useZip64)
{
    // Every test target must be able to create its archives regardless of suite order.
    EnsureZip64Dir();
    FILE *fp = fopen(hapPath.c_str(), "wb+");
    if (fp == nullptr) {
        return false;
    }
    const uint64_t baseOffset = useZip64 ? BASE_OFFSET : CLASSIC_BASE_OFFSET;
    bool ok = ftruncate(fileno(fp), static_cast<off_t>(baseOffset)) == 0;
    uint64_t cursor = baseOffset;
    if (ok) {
        ok = WriteEntryData(fp, entries, cursor);
    }
    uint64_t centralOffset = cursor;
    std::vector<uint8_t> central;
    if (ok) {
        ok = BuildCentralDirectory(entries, useZip64, badEntryName, badEntryOffset, central);
    }
    if (ok && !useZip64 && (centralOffset > UINT32_MAX || central.size() > UINT32_MAX)) {
        ok = false;
    }
    if (ok) {
        ok = WriteAt(fp, centralOffset, central.data(), central.size());
        cursor = centralOffset + central.size();
    }
    if (ok) {
        ok = WriteEndRecords(fp, cursor, useZip64, corruptLocator, entries.size(), central, centralOffset);
    }
    if (ok && fflush(fp) != 0) {
        ok = false;
    }
    if (fclose(fp) != 0) {
        ok = false;
    }
    return ok;
}

const std::vector<std::string> &MediaDensityPaths()
{
    static const std::vector<std::string> paths = {
        "resources/base/media/icon.png",
        "resources/ldpi/media/icon.png",
        "resources/mdpi/media/icon.png",
        "resources/sdpi/media/icon.png",
        "resources/xldpi/media/icon.png",
        "resources/xxldpi/media/icon.png",
        "resources/xxxldpi/media/icon.png",
    };
    return paths;
}

bool LoadTestData(const std::string &relative, std::vector<uint8_t> &out)
{
    return ReadWholeFile(TestDataPath(relative), out);
}

bool BuildBigHap(const std::string &hapPath, bool corruptLocator, const std::string &badEntryName,
    uint64_t badEntryOffset, bool useV2Index)
{
    std::vector<uint8_t> index;
    std::vector<uint8_t> media;
    std::vector<uint8_t> profile;
    std::vector<uint8_t> rawfile;
    const char *indexPath = useV2Index ? "all/assets/entry/resourcesV2.index"
                                       : "all/assets/entry/resources.index";
    if (!LoadTestData(indexPath, index) ||
        !LoadTestData("all/assets/entry/resources/base/media/icon.png", media) ||
        !LoadTestData("all/assets/entry/resources/base/profile/test_profile.json", profile) ||
        !LoadTestData("all/assets/entry/resources/rawfile/test_rawfile.txt", rawfile)) {
        return false;
    }
    std::vector<Zip64Entry> entries;
    entries.push_back({ "resources.index", index, 0 });
    for (const auto &path : MediaDensityPaths()) {
        entries.push_back({ path, media, 0 });
    }
    entries.push_back({ "resources/base/profile/test_profile.json", profile, 0 });
    entries.push_back({ "resources/rawfile/test_rawfile.txt", rawfile, 0 });
    return BuildZip64Hap(hapPath, entries, corruptLocator, badEntryName, badEntryOffset);
}

bool BuildClassicHap(const std::string &hapPath)
{
    std::vector<uint8_t> index;
    std::vector<uint8_t> media;
    std::vector<uint8_t> profile;
    std::vector<uint8_t> rawfile;
    if (!LoadTestData("all/assets/entry/resources.index", index) ||
        !LoadTestData("all/assets/entry/resources/base/media/icon.png", media) ||
        !LoadTestData("all/assets/entry/resources/base/profile/test_profile.json", profile) ||
        !LoadTestData("all/assets/entry/resources/rawfile/test_rawfile.txt", rawfile)) {
        return false;
    }
    std::vector<Zip64Entry> entries;
    entries.push_back({ "resources.index", index, 0 });
    for (const auto &path : MediaDensityPaths()) {
        entries.push_back({ path, media, 0 });
    }
    entries.push_back({ "resources/base/profile/test_profile.json", profile, 0 });
    entries.push_back({ "resources/rawfile/test_rawfile.txt", rawfile, 0 });
    return BuildZip64Hap(hapPath, entries, false, "", 0, false);
}

bool BuildOverlayHap(const std::string &hapPath)
{
    std::vector<uint8_t> index;
    if (!LoadTestData("overlay/assets/entry/resources.index", index)) {
        return false;
    }
    std::vector<Zip64Entry> entries;
    entries.push_back({ "resources.index", index, 0 });
    return BuildZip64Hap(hapPath, entries, false, "", 0);
}

bool BuildPatchHap(const std::string &hapPath, const std::string &rawfileContent)
{
    std::vector<uint8_t> index;
    if (!LoadTestData("all/assets/entry/resources.index", index)) {
        return false;
    }
    std::vector<uint8_t> rawfile(rawfileContent.begin(), rawfileContent.end());
    std::vector<Zip64Entry> entries;
    entries.push_back({ "resources.index", index, 0 });
    entries.push_back({ "resources/rawfile/test_rawfile.txt", rawfile, 0 });
    return BuildZip64Hap(hapPath, entries, false, "", 0);
}

bool BuildIndexOnlyHap(const std::string &hapPath)
{
    std::vector<uint8_t> index;
    if (!LoadTestData("all/assets/entry/resources.index", index)) {
        return false;
    }
    std::vector<Zip64Entry> entries;
    entries.push_back({ "resources.index", index, 0 });
    return BuildZip64Hap(hapPath, entries, false, "", 0);
}

bool WriteGarbageFile(const std::string &path)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }
    const char *garbage = "not a zip64 hap";
    out.write(garbage, static_cast<std::streamsize>(strlen(garbage)));
    out.close();
    return !out.fail();
}

std::shared_ptr<ResourceManager> CreateTestResourceManager()
{
    ResourceManager *rawManager = CreateResourceManager(false);
    if (rawManager == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ResourceManager> manager(rawManager);
    ResConfig *config = CreateResConfig();
    if (config == nullptr) {
        return manager;
    }
    config->SetLocaleInfo("en", nullptr, "US");
    manager->UpdateResConfig(*config);
    delete config;
    return manager;
}

LargeHapLoadResult LoadLargeHap(const std::shared_ptr<ResourceManager> &manager,
    const std::string &hapPath)
{
    if (manager != nullptr && manager->AddResource(hapPath.c_str())) {
        return LargeHapLoadResult::LOADED;
    }
    auto control = CreateTestResourceManager();
    if (control == nullptr || !control->AddResource(TestDataPath("all.hap").c_str())) {
        return LargeHapLoadResult::BROKEN;
    }
    return LargeHapLoadResult::UNSUPPORTED;
}

} // namespace Zip64Test
} // namespace Resource
} // namespace Global
} // namespace OHOS
