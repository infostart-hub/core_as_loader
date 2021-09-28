/*
* (c) Проект "Core.As", Александр Орефков orefkov@gmail.com
* Чтение информации о версии
*/
#pragma once

class VersionInfo {
public:
    VersionInfo() = default;
    VersionInfo(const wchar_t* strFilePath, BOOL bFull);

    VersionInfo(HINSTANCE hInst, BOOL bFull) {
        if (hInst)
            init(hInst, bFull);
    }

    ~VersionInfo();

    struct SBlockInfo {
        DWORD dwLangCP;
        hashStrMapWIA<stringw> Names2Vals;
        stringw getStrLanguage() const;
    };

    bool isLoaded() const {
        return loaded;
    }
    uint64_t fileVersion() const {
        return m_wFileVersion;
    }
    uint64_t prodVersion() const {
        return m_wProdVersion;
    }
    size_t blocksCount() const {
        return m_blocks.size();
    }
    SBlockInfo* block(unsigned dwIdx) const {
        return m_blocks[dwIdx];
    }

protected:
    void init(HINSTANCE hInst, BOOL bFull);
    std::vector<SBlockInfo*> m_blocks;
    uint64_t m_wFileVersion;
    uint64_t m_wProdVersion;
    bool loaded{ false };
};
