#include "TaxiMapFrame.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include "WoW/Source/DB/Rec/TaxiPathRec.h"
#include "WoW/Source/DB/Rec/TaxiPathNodeRec.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Services/Texture.h"

// STATIC: 0x00C0D7E0
CTexture* CGTaxiMap::m_mapTexture;

// STATIC: 0x00C0D7E4
int32_t CGTaxiMap::m_numNodes;

// STATIC: 0x00C0DC30
TSGrowableArray<TaxiNodeInfo> CGTaxiMap::m_nodes;

// STATIC: 0x00C0DC80
TSFixedArray<TSGrowableArray<TAXIROUTELINE>> CGTaxiMap::m_routeLines;

// STATIC: 0x00C0DBF0
WOWGUID CGTaxiMap::m_unit;

// STATIC: 0x00C9EA98
uint32_t CGTaxiMap::m_maxNodeID;

// STATIC: 0x00C9EA9C
uint32_t CGTaxiMap::m_nodeMaskSize;

// STATIC: 0x00C0DC08
static TSHashTable<TAXIPATHNODESCLIENT, HASHKEY_NONE> s_pathNodes;

// STATIC: 0x00C0DC40
static TSGrowableArray<uint64_t> s_nodeMask;

// STATIC: 0x00C0DC50
static TSFixedArray<TSGrowableArray<TaxiPathRec*>> s_paths;

// STATIC: 0x00C0DC60
static TSGrowableArray<TAXILINE> s_taxiLines;

// STATIC: 0x00C0DC70
static TSGrowableArray<uint8_t> s_unkC0DC70;

// VERIFY: d4891602 manual
// OFFSET: 0x005938D0
void CGTaxiMap::InitializeGame() {
    uint32_t maxNodeID = g_taxiNodesDB.m_maxID;
    uint32_t maskSize = (maxNodeID >> 6) + 1;

    CGTaxiMap::m_unit.m_guid = 0;
    CGTaxiMap::m_numNodes = 0;
    CGTaxiMap::m_maxNodeID = maxNodeID;
    CGTaxiMap::m_nodeMaskSize = maskSize;

    CGTaxiMap::m_nodes.SetCount(maxNodeID);
    s_nodeMask.SetCount(CGTaxiMap::m_nodeMaskSize);
    s_paths.SetCount(CGTaxiMap::m_maxNodeID);

    for (uint32_t i = 0; i < CGTaxiMap::m_maxNodeID; i++) {
        s_paths.m_data[i].SetCount(CGTaxiMap::m_maxNodeID);

        for (uint32_t j = 0; j < CGTaxiMap::m_maxNodeID; j++) {
            s_paths.m_data[i].m_data[j] = nullptr;
        }
    }

    s_unkC0DC70.SetCount(CGTaxiMap::m_maxNodeID);
    CGTaxiMap::m_routeLines.SetCount(CGTaxiMap::m_maxNodeID);

    for (int32_t i = g_taxiPathDB.m_numRecords - 1; i >= 0; i--) {
        TaxiPathRec* path = i < 0 || i >= g_taxiPathDB.m_numRecords ? nullptr : &g_taxiPathDB.m_records[i];
        TaxiNodesRec* from = g_taxiNodesDB.GetRecord(path->m_fromTaxiNode);
        TaxiNodesRec* to = g_taxiNodesDB.GetRecord(path->m_toTaxiNode);

        if (from && to) {
            s_paths.m_data[from->m_id - 1].m_data[to->m_id - 1] = path;
        }
    }

    int32_t lastPathID = -1;
    TAXIPATHNODESCLIENT* pathNodes = nullptr;

    for (int32_t i = 0; i < g_taxiPathNodeDB.m_numRecords; i++) {
        TaxiPathNodeRec* node = i < 0 || i >= g_taxiPathNodeDB.m_numRecords ? nullptr : &g_taxiPathNodeDB.m_records[i];
        int32_t pathID = node->m_pathID;

        if (pathID == lastPathID) {
            pathNodes->m_numNodes++;
        } else {
            pathNodes = s_pathNodes.New(pathID, HASHKEY_NONE(), 0, 0);
            pathNodes->m_firstNode = node;
            pathNodes->m_numNodes = 1;
            lastPathID = node->m_pathID;
        }
    }
}

// VERIFY: 7b70e52d manual
// OFFSET: 0x00591A10
void CGTaxiMap::FreeRouteLines() {
    if (CGTaxiMap::m_mapTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(CGTaxiMap::m_mapTexture));
        CGTaxiMap::m_mapTexture = nullptr;
    }

    s_taxiLines.Clear();

    for (uint32_t i = 0; i < CGTaxiMap::m_maxNodeID; i++) {
        CGTaxiMap::m_routeLines.m_data[i].Clear();
    }

    CGTaxiMap::m_numNodes = 0;
}

// VERIFY: 7911d34a auto
// OFFSET: 0x005920D0
void CGTaxiMap::ShutdownGame() {
    s_pathNodes.InternalClear(0);
    CGTaxiMap::FreeRouteLines();
}
