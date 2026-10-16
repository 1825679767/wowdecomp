// Copyright (C) 2026 WarcraftXL. GPLv3.
#pragma once
#include <cstdint>
namespace wxl::waystone::skeleton {
// Existing RetailCharacter eight ID/path authorities only. No FDID inference.
inline const char* KnownParentPath(uint32_t id){switch(id){
 case 1685880:return "character\\draenei\\male\\draeneimale_hd.skel";case 1699074:return "character\\draenei\\female\\draeneifemale_hd.skel";
 case 1838505:return "character\\bloodelf\\female\\bloodelffemale_hd.skel";case 1838675:return "character\\bloodelf\\male\\bloodelfmale_hd.skel";
 case 1839627:return "character\\nightelf\\female\\nightelffemale_hd.skel";case 1839628:return "character\\nightelf\\male\\nightelfmale_hd.skel";
 case 1892673:return "character\\dwarf\\female\\dwarffemale_hd.skel";case 1892675:return "character\\dwarf\\male\\dwarfmale_hd.skel";default:return nullptr;}}
}
