/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Чтение информации о версии
*/
#include "pch.h"
#include "VersionInfo.h"

VersionInfo::VersionInfo(const wchar_t* strFilePath, BOOL bFull) {
    HINSTANCE hInst = GetModuleHandle(strFilePath);
    if (hInst)
        init(hInst, bFull);
    else {
        hInst = LoadLibraryEx(strFilePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (hInst) {
            init(hInst, bFull);
            FreeLibrary(hInst);
        }
    }
}

VersionInfo::~VersionInfo() {
    for (const auto p: m_blocks)
        delete p;
}

void VersionInfo::init(HINSTANCE hInst, BOOL bFull) {
    HRSRC hRes = FindResource(hInst, (LPCTSTR) 1, RT_VERSION);
    if (!hRes)
        return;
    DWORD dwSize = SizeofResource(hInst, hRes);
    if (!dwSize)
        return;
    HGLOBAL hData = LoadResource(hInst, hRes);
    if (!hData)
        return;
    void* pData = LockResource(hData);
    if (!pData) {
        FreeResource(hData);
        return;
    }
    unique_ptr<char> updata(new char[dwSize]);
    char* m_pData = updata.get();
    memcpy(m_pData, pData, dwSize);
    UnlockResource(hData);
    FreeResource(hData);

    struct VS_BASE {
        WORD  wLength;
        WORD  wValueLength;
        WORD  wType;
        wchar_t* GetText() {
            return reinterpret_cast<wchar_t*>(this + 1);
        }
    };

    struct VS_VERSIONINFO : VS_BASE {
        wchar_t str[16];
        WORD padding;
        VS_FIXEDFILEINFO info;
    };

    struct StringFileInfo : VS_BASE {
        wchar_t str[sizeof("StringFileInfo")];
    };

    struct StringTable : VS_BASE {
        wchar_t str[sizeof("00000000")];
    };

    VS_VERSIONINFO* pVersion = reinterpret_cast<VS_VERSIONINFO*>(m_pData);
    m_wFileVersion = *(uint64_t*) &pVersion->info.dwFileVersionMS;
    m_wFileVersion = (m_wFileVersion >> 32) | (m_wFileVersion << 32);

    m_wProdVersion = *(uint64_t*) &pVersion->info.dwProductVersionMS;
    m_wProdVersion = (m_wProdVersion >> 32) | (m_wProdVersion << 32);
    loaded = true;
    if (!bFull)
        return;

    StringFileInfo* pSFI = reinterpret_cast<StringFileInfo*>(pVersion + 1);
    if (pSFI->str[0] == L'S') { // StringFileInfo
        StringTable* pTable = reinterpret_cast<StringTable*>(pSFI + 1);
        StringTable* pTableEnd = reinterpret_cast<StringTable*>(reinterpret_cast<size_t>(pSFI) + pSFI->wLength);
        while (pTable < pTableEnd) {
            SBlockInfo* pBlock = new SBlockInfo;
            m_blocks.push_back(pBlock);
            pBlock->dwLangCP = 0;
            wchar_t* ptr = pTable->str;
            for (int i = 0; i < 8; i++, ptr++)
                pBlock->dwLangCP = (pBlock->dwLangCP << 4) |
                (*ptr < 'A' ? *ptr - '0' : (*ptr < 'a' ? *ptr - 'A' : *ptr - 'a') + 10);
            VS_BASE* pString = reinterpret_cast<VS_BASE*>(pTable + 1);
            VS_BASE* pStringEnd = reinterpret_cast<VS_BASE*>(reinterpret_cast<size_t>(pTable) + pTable->wLength);
            while (pString < pStringEnd) {
                wchar_t* pText = pString->GetText();
                stringw key(e_s(pText));
                if (pString->wValueLength) {
                    pText += key.length() + 1;
                    pText = reinterpret_cast<wchar_t*>((reinterpret_cast<size_t>(pText) + 3) & ~3);
                } else
                    pText = nullptr;
                pBlock->Names2Vals.emplace(move(key), e_s(pText));
                pString = reinterpret_cast<VS_BASE*>((reinterpret_cast<size_t>(pString) + pString->wLength + 3) & ~3);
            }
            pTable = reinterpret_cast<StringTable*>((reinterpret_cast<size_t>(pTable) + pTable->wLength + 3) & ~3);
        }
    }
}

stringw VersionInfo::SBlockInfo::getStrLanguage() const {
    return lstringsw<100>([=](wchar_t* p, unsigned s) {
        return grow2(GetLocaleInfo(MAKELCID(dwLangCP >> 16, SORT_DEFAULT), LOCALE_SLANGUAGE, p, s +1), s);
        });
}
