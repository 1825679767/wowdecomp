// Immutable retail item metadata consumed by native DBC-first item accessors.
// Copyright (C) 2026 WarcraftXL. GPLv3.

#include "RetailItemCatalog.hpp"
#include "ItemGraph.hpp"

#include <atomic>

namespace wxl::runtime::db2::retailitems
{
    namespace
    {
        std::shared_ptr<const Catalog>& CurrentSlot()
        {
            static auto* slot = new std::shared_ptr<const Catalog>();
            return *slot;
        }
    }

    void Publish(std::shared_ptr<Catalog> catalog)
    {
        catalog->weaponSubclassByDisplay.clear();
        auto add=[&](uint32_t display,const Item& item){
            if(!display)return;
            const uint32_t subclass=item.classId==2?item.subclassId:UINT32_MAX;
            auto [it,inserted]=catalog->weaponSubclassByDisplay.emplace(display,subclass);
            if(!inserted&&it->second!=subclass)it->second=UINT32_MAX;
        };
        for(const auto& [id,item]:catalog->items)add(item.displayId,item);
        for(const auto& [key,variant]:catalog->variants){
            const auto item=catalog->items.find(variant.itemId);
            if(item!=catalog->items.end())add(variant.displayId,item->second);
        }
        std::atomic_store_explicit(&CurrentSlot(),
                                   std::shared_ptr<const Catalog>(std::move(catalog)),
                                   std::memory_order_release);
        wxl::native::ItemGraphPublished();
    }

    std::shared_ptr<const Catalog> Current()
    {
        return std::atomic_load_explicit(&CurrentSlot(), std::memory_order_acquire);
    }
}
