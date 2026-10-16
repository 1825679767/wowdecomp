// Native forward FDID resolution, adapted from wxl-db2/src/api/FdidResolver.cpp.
// Copyright (C) 2026 WarcraftXL. GPLv3 or later.
#include "FdidCompat.hpp"
#include "NativeFeatures.hpp"
#include "vendor/extensions/wxl-db2/src/api/Db2.hpp"
#include <map>
#include "vendor/extensions/wxl-db2/src/decode/DB2File.hpp"
#include "common/Log.hpp"

#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace wxl::native
{
    namespace
    {
        struct PathRow { uint32_t id; int32_t path; };

        class PathTable final : public wxl::features::db2::DB2Table<PathRow>
        {
            const uint32_t* StringColumns(uint32_t* count) const override
            {
                // With an inline ID, the path is physical field 1; with an
                // external ID list it is field 0. The decoder skips inline IDs
                // and out-of-range fields, so both layouts use the same reader.
                static constexpr uint32_t columns[] = {0, 1};
                *count = 2;
                return columns;
            }
        };

        struct Resolver
        {
            PathTable textures;
            PathTable models;
            std::once_flag loaded;
            std::mutex mutex;
            std::unordered_map<uint32_t, std::string> paths;

            void Load()
            {
                textures.Load("TextureFilePath.db2");
                models.Load("ModelFilePath.db2");
                WLOG_INFO("native-fdid: texpath=%u modelpath=%u",
                    textures.RowCount(), models.RowCount());
            }

            static const char* Find(const PathTable& table, uint32_t id)
            {
                const auto* row = table.Find(static_cast<int32_t>(id));
                return row ? table.Str(static_cast<uint32_t>(row->path)) : nullptr;
            }

            const char* Resolve(uint32_t id)
            {
                std::call_once(loaded, [this] { Load(); });
                std::lock_guard<std::mutex> lock(mutex);
                auto it = paths.find(id);
                if (it == paths.end())
                {
                    // FDIDs inhabit one global space, matching the original
                    // resolver's texture-first then model authority lookup.
                    const char* path = Find(textures, id);
                    if (!path || !*path) path = Find(models, id);
                    std::string value;
                    if (path && *path) value = path;
                    it = paths.emplace(id, std::move(value)).first;
                }
                // unordered_map rehash keeps node references and pointers
                // valid. These strings are never modified or erased.
                return it->second.empty() ? nullptr : it->second.c_str();
            }
        };

        const char* Resolve(uint32_t id) noexcept
        {
            if (!id || !AssetsEnabled()) return nullptr;
            try
            {
                // Intentionally retain the immutable authority and cache for
                // process lifetime, including callers during static teardown.
                static Resolver* const resolver = new Resolver;
                return resolver->Resolve(id);
            }
            catch (...)
            {
                // Loader entry points are noexcept and must fail closed even
                // on allocation failure or a storage/decode exception.
                return nullptr;
            }
        }
    }

    uint32_t ResolveMaterialTexture(uint32_t materialResourceId,uint32_t usage) noexcept {
        if(!materialResourceId || !AssetsEnabled())return 0;
        try {
            using namespace wxl::runtime::db2;
            struct Index {
                std::map<std::pair<uint32_t,uint32_t>,uint32_t> rows;
                Index(){
                    static constexpr Field fields[]={{"ID"},{"UsageType"},{"MaterialResourcesID"}};
                    static constexpr Definition def{"texturefiledata","DBFilesClient\\texturefiledata.db2",0xBD7C74C2u,fields};
                    Table table;std::string error;
                    if(!table.Load(def,&error)){WLOG_WARN("native material texture table: %s",error.c_str());return;}
                    for(const auto& row:table.Rows()){
                        const auto key=std::pair{table.Value(row,"MaterialResourcesID"),table.Value(row,"UsageType")};
                        auto [it,inserted]=rows.emplace(key,row.id);
                        // Ambiguous material/usage data must not silently select an unrelated sheet.
                        if(!inserted && it->second!=row.id)it->second=0;
                    }
                }
            };
            static const Index* const index=new Index;
            const auto it=index->rows.find({materialResourceId,usage});
            return it==index->rows.end()?0:it->second;
        }catch(...){return 0;}
    }
    const char* ResolveTexture(uint32_t fileDataId) noexcept { return Resolve(fileDataId); }
    const char* ResolveModel(uint32_t fileDataId) noexcept { return Resolve(fileDataId); }
}
