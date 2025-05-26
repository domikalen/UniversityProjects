#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

class CLinker
{
public:
    CLinker() = default;
    ~CLinker() = default;
    CLinker(const CLinker&) = delete;
    CLinker& operator = (const CLinker&) = delete;

    CLinker& addFile(const std::string& fileName)
    {
        std::ifstream input(fileName, std::ios::binary);
        if (!input)
            throw std::runtime_error("Cannot read input file");

        uint32_t exportCount = 0, importCount = 0, codeSize = 0;
        input.read(reinterpret_cast<char*>(&exportCount), sizeof(exportCount));
        input.read(reinterpret_cast<char*>(&importCount), sizeof(importCount));
        input.read(reinterpret_cast<char*>(&codeSize),    sizeof(codeSize));
        if (!input)
            throw std::runtime_error("Cannot read input file");

        std::vector<std::pair<std::string,uint32_t>> rawExports;
        rawExports.reserve(exportCount);
        for (uint32_t i = 0; i < exportCount; ++i){
            uint8_t len;
            input.read(reinterpret_cast<char*>(&len), 1);
            std::string name(len, '\0');
            input.read(&name[0], len);
            uint32_t off;
            input.read(reinterpret_cast<char*>(&off), 4);
            if (!input)
                throw std::runtime_error("Cannot read input file");
            rawExports.emplace_back(std::move(name), off);
        }

        std::vector<FuncImport> tmpImports;
        tmpImports.reserve(importCount);
        for (uint32_t i = 0; i < importCount; ++i){
            uint8_t len;
            input.read(reinterpret_cast<char*>(&len), 1);
            std::string name(len, '\0');
            input.read(&name[0], len);
            uint32_t cnt;
            input.read(reinterpret_cast<char*>(&cnt), 4);
            if (!input)
                throw std::runtime_error("Cannot read input file");

            FuncImport imp;
            imp.name = std::move(name);
            imp.pos.resize(cnt);
            for (uint32_t k = 0; k < cnt; ++k){
                input.read(reinterpret_cast<char*>(&imp.pos[k]), 4);
                if (!input)
                    throw std::runtime_error("Cannot read input file");
            }
            tmpImports.push_back(std::move(imp));
        }

        std::vector<uint8_t> codeBlock(codeSize);
        if (codeSize > 0){
            input.read(reinterpret_cast<char*>(codeBlock.data()), codeSize);
            if (!input)
                throw std::runtime_error("Cannot read input file");
        }

        std::sort(rawExports.begin(), rawExports.end(),[] (auto& a, auto& b) { return a.second < b.second; });

        ObjectFile of;
        of.code     = std::move(codeBlock);

        of.exports . reserve ( rawExports . size () );
        for ( size_t i = 0; i < rawExports . size (); ++ i ){
            auto & exp = rawExports [ i ];
            uint32_t nextOffset = ( i + 1 < rawExports . size () ? rawExports [ i + 1 ] . second : codeSize );
            of . exports . push_back ({exp . first,exp . second, nextOffset - exp . second });
        }

        of.imports = std::move(tmpImports);
        m_Files.push_back(std::move(of));
        return *this;
    }

    void linkOutput(const std::string& fileName,
                    const std::string& entryPoint)
    {
        struct SymLoc { size_t fileIdx, expIdx; };
        std::unordered_map<std::string,SymLoc> symMap;
        for (size_t fi = 0; fi < m_Files.size(); ++fi){
            auto& obj = m_Files[fi];
            for (size_t ei = 0; ei < obj.exports.size(); ++ei){
                const auto& sym = obj.exports[ei].name;
                if (symMap.count(sym))
                    throw std::runtime_error("Duplicate symbol: " + sym);
                symMap[sym] = { fi, ei };
            }
        }

        if (!symMap.count(entryPoint))
            throw std::runtime_error("Undefined symbol " + entryPoint);

        std::vector<std::string> queue { entryPoint };
        std::unordered_set<std::string> used { entryPoint };
        for (size_t qi = 0; qi < queue.size(); ++qi){
            const auto& sym = queue[qi];
            auto loc = symMap[sym];
            auto& obj = m_Files[loc.fileIdx];
            auto& ex  = obj.exports[loc.expIdx];
            uint32_t fstart = ex.offset, fend = ex.offset + ex.size;

            for (auto& imp : obj.imports)
                for (auto pos : imp.pos)
                    if (pos >= fstart && pos + 4 <= fend){
                        const auto& dep = imp.name;
                        if (!symMap.count(dep))
                            throw std::runtime_error("Undefined symbol " + dep);
                        if (!used.count(dep)){
                            used.insert(dep);
                            queue.push_back(dep);
                        }
                    }
        }

        std::vector<std::string> order { entryPoint }, rest;
        for (auto& s : used)
            if (s != entryPoint)
                rest.push_back(s);
        std::sort(rest.begin(), rest.end());
        order.insert(order.end(), rest.begin(), rest.end());

        std::unordered_map<std::string,uint32_t> addr;
        std::vector<uint8_t> outputData;
        uint32_t cur = 0;
        for (auto& sym : order){
            auto loc = symMap[sym];
            auto& ex  = m_Files[loc.fileIdx].exports[loc.expIdx];
            addr[sym] = cur;
            auto& code = m_Files[loc.fileIdx].code;
            outputData.insert(outputData.end(),code.begin() + ex.offset,code.begin() + ex.offset + ex.size);
            cur += ex.size;
        }

        for (auto& sym : order){
            auto loc = symMap[sym];
            auto& obj = m_Files[loc.fileIdx];
            auto& ex  = obj.exports[loc.expIdx];
            uint32_t base = addr[sym];
            for (auto& imp : obj.imports){
                uint32_t depAddr = addr[imp.name];
                for (auto pos : imp.pos)
                    if (pos >= ex.offset && pos + 4 <= ex.offset + ex.size){
                        uint32_t patchAt = base + (pos - ex.offset);
                        for (int b = 0; b < 4; ++b)
                            outputData[patchAt + b] = (depAddr >> (8 * b)) & 0xFF;
                    }
            }
        }

        std::ofstream output(fileName, std::ios::binary);
        if (!output)
            throw std::runtime_error("Cannot write output file");
        output.write(reinterpret_cast<const char*>(outputData.data()),outputData.size());
        if (!output)
            throw std::runtime_error("Cannot write output file");
    }

private:
    struct FuncExport{
        std::string name;
        uint32_t    offset, size;
    };
    struct FuncImport{
        std::string          name;
        std::vector<uint32_t> pos;
    };
    struct ObjectFile{
        std::vector<uint8_t>       code;
        std::vector<FuncExport>    exports;
        std::vector<FuncImport>    imports;
    };
    std::vector<ObjectFile> m_Files;
};


#ifndef __PROGTEST__
int main ()
{
    CLinker () . addFile ( "0in0.o" ) . linkOutput ( "0out", "strlen" );

    CLinker () . addFile ( "1in0.o" ) . linkOutput ( "1out", "main" );

    CLinker () . addFile ( "2in0.o" ) . addFile ( "2in1.o" ) . linkOutput ( "2out", "main" );

    CLinker () . addFile ( "3in0.o" ) . addFile ( "3in1.o" ) . linkOutput ( "3out", "towersOfHanoi" );

    try
    {
        CLinker () . addFile ( "4in0.o" ) . addFile ( "4in1.o" ) . linkOutput ( "4out", "unusedFunc" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Undefined symbol qsort
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "5in0.o" ) . linkOutput ( "5out", "main" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Duplicate symbol: printf
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "6in0.o" ) . linkOutput ( "6out", "strlen" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Cannot read input file
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    try
    {
        CLinker () . addFile ( "7in0.o" ) . linkOutput ( "7out", "strlen2" );
        assert ( "missing an exception" == nullptr );
    }
    catch ( const std::runtime_error & e )
    {
        // e . what (): Undefined symbol strlen2
    }
    catch ( ... )
    {
        assert ( "invalid exception" == nullptr );
    }

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

