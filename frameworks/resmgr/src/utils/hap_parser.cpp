/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hap_parser.h"

#include <algorithm>
#include <cstdlib>
#include <string>
#include <fcntl.h>
#include <unzip.h>
#include <unistd.h>
#include <set>
#include <sys/stat.h>
#include <unordered_map>

#include "hilog_wrapper.h"
#include "locale_matcher.h"
#if defined(__WINNT__)
#include <cstring>
#else
#include "securec.h"
#endif
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
#include "hitrace_meter.h"
#endif
#include "utils/errors.h"
#include "utils/string_utils.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
const char *HapParser::RES_FILE_NAME = "/resources.index";
const std::string NOT_DEVICE_TYPE = "not_device_type";
const std::string DEVICE_DEFAULT = "default";
static const std::unordered_map<ResType, uint32_t> TYPE_MAP {
    {INTEGER, SELECT_INTEGER},
    {STRING, SELECT_STRING},
    {STRINGARRAY, SELECT_STRINGARRAY},
    {INTARRAY, SELECT_INTARRAY},
    {BOOLEAN, SELECT_BOOLEAN},
    {COLOR, SELECT_COLOR},
    {THEME, SELECT_THEME},
    {PLURALS, SELECT_PLURALS},
    {FLOAT, SELECT_FLOAT},
    {MEDIA, SELECT_MEDIA},
    {PROF, SELECT_PROF},
    {PATTERN, SELECT_PATTERN},
    {SYMBOL, SELECT_SYMBOL}
};

int32_t LocateFile(unzFile &uf, const char *fileName)
{
    if (unzLocateFile2(uf, fileName, 1)) { // try to locate file inside zip, 1 = case sensitive
        return UNKNOWN_ERROR;
    }
    return OK;
}

int32_t GetCurrentFileInfo(unzFile &uf, unz_file_info &fileInfo)
{
    // obtained the necessary details about file inside zip
    char filenameInzip[256];  // for unzGetCurrentFileInfo
    int err = unzGetCurrentFileInfo(uf, &fileInfo, filenameInzip, sizeof(filenameInzip), nullptr, 0, nullptr, 0);
    if (err != UNZ_OK) {
        RESMGR_HILOGE(RESMGR_TAG, "GetCurrentFileInfo failed");
        return UNKNOWN_ERROR;
    }
    return OK;
}

int32_t ReadCurrentFile(unzFile &uf, unz_file_info &fileInfo, std::unique_ptr<uint8_t[]> &buffer,
    size_t &bufLen)
{
    buffer = std::make_unique<uint8_t[]>(fileInfo.uncompressed_size);
    bufLen = fileInfo.uncompressed_size;
    if (buffer == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "Error allocating memory for read buffer");
        return UNKNOWN_ERROR;
    }

    int err = unzOpenCurrentFilePassword(uf, nullptr);
    if (err != UNZ_OK) {
        RESMGR_HILOGE(RESMGR_TAG, "Error %d in unzOpenCurrentFilePassword.", err);
        return UNKNOWN_ERROR;
    } // file inside the zip is open

    err = unzReadCurrentFile(uf, buffer.get(), bufLen);
    if (err < 0) {
        RESMGR_HILOGE(RESMGR_TAG, "Error %d in unzReadCurrentFile", err);
        return UNKNOWN_ERROR;
    }

    return OK;
}

int32_t HapParser::ReadFileFromZip(unzFile &uf, const char *fileName, std::unique_ptr<uint8_t[]> &buffer,
    size_t &bufLen)
{
    unz_file_info fileInfo;
    if (LocateFile(uf, fileName) != OK) {
        return UNKNOWN_ERROR;
    }
    if (GetCurrentFileInfo(uf, fileInfo) != OK) {
        return UNKNOWN_ERROR;
    }
    if (ReadCurrentFile(uf, fileInfo, buffer, bufLen) != OK) {
        return UNKNOWN_ERROR;
    }
    return OK;
}

std::string GetModuleName(const char *configStr, size_t len)
{
    if (configStr == nullptr) {
        return std::string();
    }
    std::string config(configStr, len);
    static const char *key = "\"moduleName\"";
    auto idx = config.find(key);
    if (idx == std::string::npos) {
        return std::string();
    }
    auto start = config.find("\"", idx + strlen(key));
    if (start == std::string::npos) {
        return std::string();
    }
    auto end = config.find("\"", start + 1);
    if (end == std::string::npos) {
        return std::string();
    }

    if (end < start + 1) {
        return std::string();
    }
    std::string retStr = std::string(configStr + start + 1, end - start - 1);
    return retStr;
}

bool HapParser::IsStageMode(unzFile &uf)
{
    // stage mode contains "module.json", The 1 means the case sensitive
    if (unzLocateFile2(uf, "module.json", 1) != UNZ_OK) {
        return false;
    }
    return true;
}

std::string ParseModuleNameFromHap(unzFile &uf)
{
    std::unique_ptr<uint8_t[]> tmpBuf;
    int32_t ret = UNZ_OK;
    size_t tmpLen;
    ret = HapParser::ReadFileFromZip(uf, "config.json", tmpBuf, tmpLen);
    if (ret != OK) {
        RESMGR_HILOGE(RESMGR_TAG, "read config.json error");
        return std::string();
    }
    // parse config.json
    std::string mName = GetModuleName(reinterpret_cast<char *>(tmpBuf.get()), tmpLen);
    if (mName.size() == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "parse moduleName from config.json error");
        return std::string();
    }
    return mName;
}

std::string GetIndexFilePath(unzFile uf)
{
    std::string mName = ParseModuleNameFromHap(uf);
    std::string indexFilePath = std::string("assets/");
    indexFilePath.append(mName);
    indexFilePath.append("/resources.index");
    return indexFilePath;
}

int32_t ReadFileInfoFromZip(unzFile &uf, const char *fileName, std::unique_ptr<uint8_t[]> &buffer, size_t &bufLen)
{
    int err = HapParser::ReadFileFromZip(uf, fileName, buffer, bufLen);
    if (err < 0) {
        unzClose(uf);
        return UNKNOWN_ERROR;
    }
    unzClose(uf);
    return OK;
}

int32_t HapParser::ReadIndexFromFile(const char *zipFile, std::unique_ptr<uint8_t[]> &buffer,
    size_t &bufLen)
{
    unzFile uf = unzOpen64(zipFile);
    if (uf == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "Error open %{public}s in ReadIndexFromFile %{public}d", zipFile, errno);
        return UNKNOWN_ERROR;
    } // file is open
    if (IsStageMode(uf)) {
        return ReadFileInfoFromZip(uf, "resources.index", buffer, bufLen);
    }
    std::string indexFilePath = GetIndexFilePath(uf);
    return ReadFileInfoFromZip(uf, indexFilePath.c_str(), buffer, bufLen);
}

std::string HapParser::GetPath(const std::string &filePath, std::string &rawFilePath)
{
    std::string tempName = filePath;
    const std::string rawFileDirName = "rawfile/";
    if (tempName.length() <= rawFileDirName.length()
    || (tempName.compare(0, rawFileDirName.length(), rawFileDirName) != 0)) {
        tempName = rawFileDirName + tempName;
    }
    rawFilePath.append(tempName);
    return rawFilePath;
}

#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
std::string HapParser::ParseModuleName(std::shared_ptr<AbilityBase::Extractor> &extractor)
{
    if (extractor == nullptr) {
        return std::string();
    }
    std::unique_ptr<uint8_t[]> configBuf;
    size_t len;
    bool ret = extractor->ExtractToBufByName("config.json", configBuf, len);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get config data from ability");
        return std::string();
    }
    // parse config.json
    std::string mName = GetModuleName(reinterpret_cast<char *>(configBuf.get()), len);
    if (mName.size() == 0) {
        RESMGR_HILOGE(RESMGR_TAG, "parse moduleName from config.json error");
        return std::string();
    }
    return mName;
}

std::string GetRawFilePathFromFA(std::shared_ptr<AbilityBase::Extractor> &extractor,
    const std::string &filePath)
{
    std::string moduleName = HapParser::ParseModuleName(extractor);
    std::string rawFilePath("assets/");
    rawFilePath.append(moduleName);
    rawFilePath.append("/resources/");
    HapParser::GetPath(filePath, rawFilePath);
    return rawFilePath;
}

std::string GetRawFilePathFromStage(const std::string &filePath)
{
    std::string rawFilePath("resources/");
    HapParser::GetPath(filePath, rawFilePath);
    return rawFilePath;
}

std::string HapParser::GetRawFilePath(std::shared_ptr<AbilityBase::Extractor> &extractor,
    const std::string &rawFileName)
{
    std::string rawfilePath;
    if (extractor->IsStageModel()) {
        rawfilePath = GetRawFilePathFromStage(rawFileName);
    } else {
        rawfilePath = GetRawFilePathFromFA(extractor, rawFileName);
    }
    return rawfilePath;
}
#endif

RState HapParser::ReadRawFileFromHap(const std::string &hapPath, const std::string &patchPath,
    const std::string &rawFileName, size_t &len, std::unique_ptr<uint8_t[]> &outValue)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    std::string tempPath = patchPath.empty() ? hapPath : patchPath;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(tempPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", tempPath.c_str());
        return NOT_FOUND;
    }
    std::string rawfilePath = HapParser::GetRawFilePath(extractor, rawFileName);
    if (!extractor->HasEntry(rawfilePath) && patchPath.empty()) {
        RESMGR_HILOGD(RESMGR_TAG,
            "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), tempPath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    if (!extractor->HasEntry(rawfilePath) && !patchPath.empty()) {
        extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
        if (extractor == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", tempPath.c_str());
            return NOT_FOUND;
        }
        rawfilePath = HapParser::GetRawFilePath(extractor, rawFileName);
        if (!extractor->HasEntry(rawfilePath)) {
            RESMGR_HILOGD(RESMGR_TAG,
                "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), tempPath.c_str());
            return ERROR_CODE_RES_PATH_INVALID;
        }
    }
    bool ret = extractor->ExtractToBufByName(rawfilePath, outValue, len);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawfile data rawfilePath, %{public}s, hapPath, %{public}s",
            rawfilePath.c_str(), tempPath.c_str());
        return NOT_FOUND;
    }
#endif
    return SUCCESS;
}

RState HapParser::ReadRawFileDescriptor(const char *hapPath, const char *patchPath, const std::string &rawFileName,
    ResourceManager::RawFileDescriptor &descriptor)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    char outPath[PATH_MAX + 1] = {0};
    std::string sPatchPath(patchPath);
    const char *tempPath = sPatchPath.empty() ? hapPath : patchPath;
    Utils::CanonicalizePath(tempPath, outPath, PATH_MAX);
    bool isNewExtractor = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(outPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor in ReadRawFileDescriptor hapPath, %{public}s", outPath);
        return NOT_FOUND;
    }
    std::string rawfilePath = HapParser::GetRawFilePath(extractor, rawFileName);
    if (!extractor->HasEntry(rawfilePath) && sPatchPath.empty()) {
        RESMGR_HILOGD(RESMGR_TAG,
            "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), outPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
    if (!extractor->HasEntry(rawfilePath) && !sPatchPath.empty()) {
        Utils::CanonicalizePath(hapPath, outPath, PATH_MAX);
        extractor = AbilityBase::ExtractorUtil::GetExtractor(outPath, isNewExtractor);
        if (extractor == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", outPath);
            return NOT_FOUND;
        }
        rawfilePath = HapParser::GetRawFilePath(extractor, rawFileName);
        if (!extractor->HasEntry(rawfilePath)) {
            RESMGR_HILOGD(RESMGR_TAG,
                "the rawfile file %{public}s is not exist in %{public}s", rawfilePath.c_str(), outPath);
            return ERROR_CODE_RES_PATH_INVALID;
        }
    }
    AbilityBase::FileInfo fileInfo;
    bool ret = extractor->GetFileInfo(rawfilePath, fileInfo);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawFileDescriptor rawfilePath, %{public}s", rawfilePath.c_str());
        return NOT_FOUND;
    }
    int zipFd = open(outPath, O_RDONLY);
    if (zipFd < 0) {
        RESMGR_HILOGE(RESMGR_TAG, "failed open file %{public}s", outPath);
        return NOT_FOUND;
    }
    descriptor.offset = static_cast<long>(fileInfo.offset);
    descriptor.length = static_cast<long>(fileInfo.length);
    descriptor.fd = zipFd;
#endif
    return SUCCESS;
}

RState HapParser::GetRawFileList(const std::string &hapPath, const std::string &rawDirPath,
    std::set<std::string>& fileSet)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    bool isNewExtractor = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG,
            "failed to get extractor from ability in GetRawFileList hapPath, %{public}s", hapPath.c_str());
        return NOT_FOUND;
    }
    std::string rawfilePath = HapParser::GetRawFilePath(extractor, rawDirPath);
    if (!extractor->IsDirExist(rawfilePath)) {
        RESMGR_HILOGD(RESMGR_TAG,
            "the rawfile dir %{public}s is not exist in %{public}s", rawfilePath.c_str(), hapPath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
    bool ret = extractor->GetFileList(rawfilePath, fileSet);
    if (!ret) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get fileSet from ability rawfilePath, %{public}s", rawfilePath.c_str());
        return ERROR_CODE_RES_PATH_INVALID;
    }
#endif
    return SUCCESS;
}

RState HapParser::GetRawFileListUnCompressed(const std::string &indexPath, const std::string &rawDirPath,
    std::vector<std::string>& fileList)
{
#ifdef __WINNT__
    char seperator = '\\';
#else
    char seperator = '/';
#endif
    auto pos = indexPath.rfind(seperator);
    if (pos == std::string::npos) {
        return ERROR_CODE_RES_PATH_INVALID;
    }
    std::string rawFilePath = indexPath.substr(0, pos) + "/resources/";
    HapParser::GetPath(rawDirPath, rawFilePath);
    return Utils::GetFiles(rawFilePath, fileList);
}

/**
 *
 * @param buffer
 * @param offset
 * @param bufLen
 * @param id
 * @param includeTemi dose length include '\0'
 * @return OK or ERROR
 */
int32_t ParseString(const char *buffer, uint32_t &offset, const size_t &bufLen, std::string &id,
    bool includeTemi = true)
{
    uint16_t strLen;
    if (offset + IdItem::SIZE_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString length failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&strLen, sizeof(strLen), buffer + offset, IdItem::SIZE_LEN);
    if (eret != OK || (includeTemi && strLen == 0)) {
        return SYS_ERROR;
    }
    offset += IdItem::SIZE_LEN; // Offset value plus 2
    if (offset + (includeTemi ? (strLen - 1) : strLen) > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseString value failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    std::string tmp = std::string(const_cast<char *>(buffer) + offset, includeTemi ? (strLen - 1) : strLen);
    offset += includeTemi ? strLen : (strLen + 1);
    id = tmp;
    return OK;
}

/**
 *
 * @param buffer
 * @param offset
 * @param bufLen
 * @param values
 * @return
 */
int32_t ParseStringArray(const char *buffer, uint32_t &offset, const size_t &bufLen, std::vector<std::string> &values)
{
    uint16_t arrLen;
    if (offset + IdItem::SIZE_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseStringArray failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&arrLen, sizeof(arrLen), buffer + offset, IdItem::SIZE_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += IdItem::SIZE_LEN; // Offset value plus 2
    // next arrLen bytes are several strings. then after, is one '\0'
    uint32_t startOffset = offset;
    std::string value;
    while (true) {
        int32_t ret = ParseString(buffer, offset, bufLen, value, false);
        if (ret != OK) {
            return ret;
        }
        values.push_back(value);

        uint32_t readSize = offset - startOffset;
        if (readSize + 1 == arrLen) {
            offset += 1; // after arrLen, got '\0'
            break;
        }
        if (readSize + 1 > arrLen) {
            // size not match, cannot > arrLen
            return SYS_ERROR;
        }
    }

    return OK;
}

uint32_t ConvertType(ResType type)
{
    auto it = TYPE_MAP.find(type);
    if (it == TYPE_MAP.end()) {
        return SELECT_ALL;
    }
    return it->second;
}

int32_t ParseIdItem(const char *buffer, uint32_t &offset, const size_t &bufLen, std::shared_ptr<IdItem> idItem,
    const uint32_t &selectedTypes)
{
    if (offset + IdItem::HEADER_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse IdItemHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(idItem.get(), sizeof(IdItem), buffer + offset, IdItem::HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    if (idItem->resType_ >= ResType::MAX_RES_TYPE) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse idItem failed, the resType: %{public}u is out of bounds",
            static_cast<uint32_t>(idItem->resType_));
        return SYS_ERROR;
    }

    if (selectedTypes != SELECT_ALL && (selectedTypes & ConvertType(idItem->resType_)) == 0) {
        return OK;
    }
    offset += IdItem::HEADER_LEN;

    idItem->JudgeArray();
    if (idItem->isArray_) {
        int32_t ret = ParseStringArray(buffer, offset, bufLen, idItem->values_);
        if (ret != OK) {
            return ret;
        }
    } else {
        int32_t ret = ParseString(buffer, offset, bufLen, idItem->value_);
        if (ret != OK) {
            return ret;
        }
        idItem->valueLen_ = idItem->value_.size();
    }
    int32_t ret = ParseString(buffer, offset, bufLen, idItem->name_);
    if (ret != OK) {
        return ret;
    }
    return OK;
}

int32_t ParseId(const char *buffer, uint32_t &offset, const size_t &bufLen, std::shared_ptr<ResId> id,
    const uint32_t &selectedTypes)
{
    if (offset + ResId::RESID_HEADER_LEN > bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResIdHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(id.get(), sizeof(ResId), buffer + offset, ResId::RESID_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResId::RESID_HEADER_LEN;
    if (id->tag_[ArrayIndex::INDEX_ZERO] != 'I' || id->tag_[ArrayIndex::INDEX_ONE] != 'D'
        || id->tag_[ArrayIndex::INDEX_TWO] != 'S' || id->tag_[ArrayIndex::INDEX_THREE] != 'S') {
        return -1;
    }
    for (uint32_t i = 0; i < id->count_; ++i) {
        std::shared_ptr<IdParam> ip = std::make_shared<IdParam>();
        if (ip == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new IdParam failed when ParseId");
            return SYS_ERROR;
        }
        if (offset + ResId::IDPARAM_HEADER_LEN > bufLen) {
            RESMGR_HILOGE(RESMGR_TAG, "Parse IdParam failed, the offset will be out of bounds");
            return SYS_ERROR;
        }
        errno_t eret = memcpy_s(ip.get(), sizeof(IdParam), buffer + offset, ResId::IDPARAM_HEADER_LEN);
        if (eret != OK) {
            return SYS_ERROR;
        }
        offset += ResId::IDPARAM_HEADER_LEN;
        std::shared_ptr<IdItem> idItem = std::make_shared<IdItem>();
        if (idItem == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new IdItem failed when ParseId");
            return SYS_ERROR;
        }
        uint32_t ipOffset = ip->offset_;
        int32_t ret = ParseIdItem(buffer, ipOffset, bufLen, idItem, selectedTypes);
        if (ret != OK) {
            return ret;
        }
        ip->idItem_ = idItem;
        id->idParams_.push_back(ip);
    }

    return OK;
}

int32_t ParseKeyParam(ParserContext &context, uint32_t &offset, bool &match, std::shared_ptr<KeyParam> &kp)
{
    kp = std::make_shared<KeyParam>();
    if (kp == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseKeyParam new KeyParam failed");
        return SYS_ERROR;
    }
    if (offset + ResKey::KEYPARAM_HEADER_LEN > context.bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "ParseKeyParam failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(kp.get(), sizeof(KeyParam), context.buffer + offset, ResKey::KEYPARAM_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResKey::KEYPARAM_HEADER_LEN;
    kp->InitStr();
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    auto resDeviceType = kp->GetDeviceTypeStr();
    context.deviceTypes.push_back(context.deviceType);
    if (context.deviceType != DEVICE_DEFAULT && resDeviceType != NOT_DEVICE_TYPE &&
        find(context.deviceTypes.begin(), context.deviceTypes.end(), resDeviceType) == context.deviceTypes.end()) {
        match = false;
    }
#endif
    return OK;
}

void GetLimitKeyValue(uint32_t &limitKeyValue, KeyType type, std::vector<bool> &keyTypes)
{
    const uint32_t limitKeysBase = 0x00000001;
    uint32_t typeValue = static_cast<uint32_t>(type);
    if (type < KeyType::KEY_TYPE_MAX && !keyTypes[typeValue]) {
        keyTypes[typeValue] = true;
        limitKeyValue |= limitKeysBase << typeValue;
    }
}

void GetKeyParamsLocales(std::shared_ptr<KeyParam> kp, std::string &locale, bool &isLocale)
{
    KeyType keyType = kp->type_;
    if (keyType == KeyType::MCC || keyType == KeyType::MNC) {
        return;
    }
    if (keyType == KeyType::LANGUAGES) {
        locale = kp->GetStr();
        isLocale = true;
        return;
    }
    if (keyType == KeyType::SCRIPT) {
        locale.append("-");
        locale.append(kp->GetStr());
        return;
    }
    if (keyType == KeyType::REGION) {
        locale.append("-");
        locale.append(kp->GetStr());
    }
}

bool SkipParseItem(ParserContext &context, const std::shared_ptr<ResKey> &key, bool &match)
{
    if (!match || (context.selectedTypes != SELECT_ALL && context.defaultConfig &&
                      !context.defaultConfig->Match(key->resConfig_, false))) {
        return true;
    }

    if (context.loadAll) {
        return false;
    }
    if (!key->resConfig_->GetLocaleInfo()) {
        return context.isUpdate;
    }

    if (context.defaultConfig && context.defaultConfig->MatchLocal(*key->resConfig_)) {
        return false;
    }

    return true;
}

int32_t ParseKey(ParserContext &context, uint32_t &offset, std::shared_ptr<ResKey> key,
    bool &match, std::vector<bool> &keyTypes)
{
    if (offset + ResKey::RESKEY_HEADER_LEN > context.bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResKeyHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(key.get(), sizeof(ResKey), context.buffer + offset, ResKey::RESKEY_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += ResKey::RESKEY_HEADER_LEN;
    if (key->tag_[ArrayIndex::INDEX_ZERO] != 'K' || key->tag_[ArrayIndex::INDEX_ONE] != 'E'
        || key->tag_[ArrayIndex::INDEX_TWO] != 'Y' || key->tag_[ArrayIndex::INDEX_THREE] != 'S') {
        return -1;
    }
    std::string locale;
    bool isLocale = false;
    std::vector<std::shared_ptr<KeyParam>> keyParams;
    for (uint32_t i = 0; i < key->keyParamsCount_; ++i) {
        std::shared_ptr<KeyParam> kp;
        if (ParseKeyParam(context, offset, match, kp) != OK) {
            return SYS_ERROR;
        }
        if (kp == nullptr) {
            return SYS_ERROR;
        }
        GetLimitKeyValue(context.limitKeyValue, kp->type_, keyTypes);
        GetKeyParamsLocales(kp, locale, isLocale);
        keyParams.push_back(kp);
    }
    if (isLocale) {
        context.locales.emplace(locale);
    }
    key->resConfig_ = HapParser::CreateResConfigFromKeyParams(keyParams);
    if (SkipParseItem(context, key, match)) {
        match = false;
        return OK;
    }

    uint32_t idOffset = key->offset_;
    std::shared_ptr<ResId> id = std::make_shared<ResId>();
    if (id == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResId failed when ParseKey");
        return SYS_ERROR;
    }
    int32_t ret = ParseId(context.buffer, idOffset, context.bufLen, id, context.selectedTypes);
    if (ret != OK) {
        return ret;
    }
    key->resId_ = id;
    return OK;
}

int32_t HapParser::ParseResHex(ParserContext &context)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
#endif
    ResHeader resHeader;
    uint32_t offset = 0;
    if (offset + RES_HEADER_LEN > context.bufLen) {
        RESMGR_HILOGE(RESMGR_TAG, "Parse ResHeader failed, the offset will be out of bounds");
        return SYS_ERROR;
    }
    errno_t eret = memcpy_s(&resHeader, sizeof(ResHeader), context.buffer + offset, RES_HEADER_LEN);
    if (eret != OK) {
        return SYS_ERROR;
    }
    offset += RES_HEADER_LEN;
    if (resHeader.keyCount_ == 0 || resHeader.length_ == 0) {
        return UNKNOWN_ERROR;
    }
    context.deviceType = ResConfigImpl::GetCurrentDeviceType();
    if (context.deviceType == std::string(TABLET_STR) || context.deviceType == std::string(TWOINONE_STR)) {
        context.deviceTypes = ResConfigImpl::GetAppSupportDeviceTypes();
    }
    std::vector<bool> keyTypes(KeyType::KEY_TYPE_MAX, false);
    for (uint32_t i = 0; i < resHeader.keyCount_; i++) {
        std::shared_ptr<ResKey> key = std::make_shared<ResKey>();
        if (key == nullptr) {
            RESMGR_HILOGE(RESMGR_TAG, "new ResKey failed when ParseResHex");
            return SYS_ERROR;
        }
        bool match = true;
        int32_t ret = ParseKey(context, offset, key, match, keyTypes);
        if (ret != OK) {
            return ret;
        }
        if (match) {
            context.resDesc.keys_.push_back(key);
        }
    }
    return OK;
}

std::shared_ptr<ResConfigImpl> HapParser::CreateResConfigFromKeyParams(
    const std::vector<std::shared_ptr<KeyParam>> &keyParams)
{
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResConfigImpl failed when CreateResConfigFromKeyParams");
        return nullptr;
    }
    size_t len = keyParams.size();
    // default path
    if (len == 0) {
        resConfig->SetColorMode(COLOR_MODE_NOT_SET);
        return resConfig;
    }
    size_t i = 0;
    ResConfigKey configKey;
    for (i = 0; i < len; ++i) {
        const std::shared_ptr<KeyParam> kp = keyParams.at(i);
        if (kp->type_ == LANGUAGES) {
            configKey.language = kp->GetStr().c_str();
        } else if (kp->type_ == REGION) {
            configKey.region = kp->GetStr().c_str();
        } else if (kp->type_ == SCRIPT) {
            configKey.script = kp->GetStr().c_str();
        } else if (kp->type_ == SCREEN_DENSITY) {
            configKey.screenDensity = GetScreenDensity(kp->value_);
        } else if (kp->type_ == DEVICETYPE) {
            configKey.deviceType = GetDeviceType(kp->value_);
        } else if (kp->type_ == DIRECTION) {
            if (kp->value_ == 0) {
                configKey.direction = DIRECTION_VERTICAL;
            } else {
                configKey.direction = DIRECTION_HORIZONTAL;
            }
        } else if (kp->type_ == INPUTDEVICE) {
            configKey.inputDevice = GetInputDevice(kp->value_);
        } else if (kp->type_ == COLORMODE) {
            configKey.colorMode = GetColorMode(kp->value_);
        } else if (kp->type_ == MCC) {
            configKey.mcc = GetMcc(kp->value_);
        } else if (kp->type_ == MNC) {
            configKey.mnc = GetMnc(kp->value_);
        }
    }

    return BuildResConfig(&configKey);
}

std::shared_ptr<ResConfigImpl> HapParser::BuildResConfig(ResConfigKey *configKey)
{
    if (configKey == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "configKey is null");
        return nullptr;
    }
    auto resConfig = std::make_shared<ResConfigImpl>();
    if (resConfig == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "new ResConfigImpl failed when BuildResConfig");
        return nullptr;
    }
    resConfig->SetDeviceType(configKey->deviceType);
    resConfig->SetDirection(configKey->direction);
    resConfig->SetColorMode(configKey->colorMode);
    resConfig->SetMcc(configKey->mcc);
    resConfig->SetMnc(configKey->mnc);
    resConfig->SetInputDevice(configKey->inputDevice);
    resConfig->SetScreenDensity((configKey->screenDensity) / Utils::DPI_BASE);
    RState r = resConfig->SetLocaleInfo(configKey->language, configKey->script, configKey->region);
    if (r != SUCCESS) {
        RESMGR_HILOGE(RESMGR_TAG,
            "error set locale,lang %s,script %s,region %s", configKey->language, configKey->script, configKey->region);
    }

    return resConfig;
}

DeviceType HapParser::GetDeviceType(uint32_t value)
{
    DeviceType deviceType = DEVICE_NOT_SET;
    if (value == DEVICE_CAR) {
        deviceType = DEVICE_CAR;
    } else if (value == DEVICE_PAD) {
        deviceType = DEVICE_PAD;
    } else if (value == DEVICE_PHONE) {
        deviceType = DEVICE_PHONE;
    } else if (value == DEVICE_TABLET) {
        deviceType = DEVICE_TABLET;
    } else if (value == DEVICE_TV) {
        deviceType = DEVICE_TV;
    } else if (value == DEVICE_WEARABLE) {
        deviceType = DEVICE_WEARABLE;
    } else if (value == DEVICE_TWOINONE) {
        deviceType = DEVICE_TWOINONE;
    }
    return deviceType;
}

uint32_t HapParser::GetMcc(uint32_t value)
{
    return value;
}

uint32_t HapParser::GetMnc(uint32_t value)
{
    return value;
}

ColorMode HapParser::GetColorMode(uint32_t value)
{
    ColorMode colorMode = COLOR_MODE_NOT_SET;
    if (value == DARK) {
        colorMode = DARK;
    } else {
        colorMode = LIGHT;
    }
    return colorMode;
}

InputDevice HapParser::GetInputDevice(uint32_t value)
{
    InputDevice inputDevice = INPUTDEVICE_NOT_SET;
    if (value == INPUTDEVICE_POINTINGDEVICE) {
        inputDevice = INPUTDEVICE_POINTINGDEVICE;
    }
    return inputDevice;
}

ScreenDensity HapParser::GetScreenDensity(uint32_t value)
{
    ScreenDensity screenDensity = SCREEN_DENSITY_NOT_SET;
    if (value == SCREEN_DENSITY_SDPI) {
        screenDensity = SCREEN_DENSITY_SDPI;
    } else if (value == SCREEN_DENSITY_MDPI) {
        screenDensity = SCREEN_DENSITY_MDPI;
    } else if (value == SCREEN_DENSITY_LDPI) {
        screenDensity = SCREEN_DENSITY_LDPI;
    } else if (value == SCREEN_DENSITY_XLDPI) {
        screenDensity = SCREEN_DENSITY_XLDPI;
    } else if (value == SCREEN_DENSITY_XXLDPI) {
        screenDensity = SCREEN_DENSITY_XXLDPI;
    } else if (value == SCREEN_DENSITY_XXXLDPI) {
        screenDensity = SCREEN_DENSITY_XXXLDPI;
    }
    return screenDensity;
}

RState HapParser::IsRawDirFromHap(const char *hapPath, const std::string &pathName, bool &outValue)
{
#if !defined(__WINNT__) && !defined(__IDE_PREVIEW__) && !defined(__ARKUI_CROSS__)
    if (pathName.empty()) {
        RESMGR_HILOGE(RESMGR_TAG, "the rawfile path is empty");
        return ERROR_CODE_RES_PATH_INVALID;
    }
    bool isNewExtractor = false;
    auto extractor = AbilityBase::ExtractorUtil::GetExtractor(hapPath, isNewExtractor);
    if (extractor == nullptr) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get extractor hapPath, %{public}s", hapPath);
        return NOT_FOUND;
    }
    std::string rawPath = HapParser::GetRawFilePath(extractor, pathName);
    if (extractor->HasEntry(rawPath)) {
        outValue = false;
    } else if (extractor->IsDirExist(rawPath)) {
        outValue = true;
    } else {
        RESMGR_HILOGD(RESMGR_TAG, "the rawfile file %{public}s is not exist in %{public}s", rawPath.c_str(), hapPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
#endif
    return SUCCESS;
}

RState HapParser::IsRawDirUnCompressed(const std::string &pathName, bool &outValue)
{
    char outPath[PATH_MAX + 1] = {0};
    Utils::CanonicalizePath(pathName.c_str(), outPath, PATH_MAX);
    struct stat fileStat {};
    if (stat(outPath, &fileStat) != 0) {
        RESMGR_HILOGE(RESMGR_TAG, "failed to get rawfile file info, %{public}s", outPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
    if ((fileStat.st_mode & S_IFDIR)) {
        outValue = true;
    } else if ((fileStat.st_mode & S_IFREG)) {
        outValue = false;
    } else {
        RESMGR_HILOGE(RESMGR_TAG, "the rawfile file %{public}s is not exist", outPath);
        return ERROR_CODE_RES_PATH_INVALID;
    }
    return SUCCESS;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
