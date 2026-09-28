#include "World.h"
#include "WoW/Source/WorldClient/SortTable.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include <cstdlib>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "Engine/Source/Os/System.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/ShadowQuery.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/Zones.h"
#include "WoW/Source/WorldClient/Particulate.h"
#include "WoW/Common/AaBsp.h"
#include <algorithm>
#include <cmath>
#include "Engine/Source/Base/RCString.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/ClipInfo.h"
#include "Engine/Source/Model2/M2Light.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapLoad.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CPortalView.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/CWorldOcclusion.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Gx/Types.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Gxu/GxuLight.h"

// STATIC: 0x00ADEE48
static const char* s_zonePaths[11] = {
    "L 415.58442,531.25541 L 387.74026,529.59307 L 385.80086,526.82251 L 385.7316,520.17316 L 375.1342,520.8658 L 374.64936,522.04329 L 373.4026,522.11255 L 372.36364,520.8658 L 365.57576,520.72727 L 365.43723,519.06493 L 348.67533,518.78788 L 343.27273,515.87879 L 334.96104,515.87879 L 333.2987,517.26406 L 319.03031,517.54113 L 243.94805,475.29004 L 225.9394,566.44155 L 266.25109,632.93506 L 350.19914,653.71429 L 415.47711,629.01747 L 416.3857,586.28568 L 417.80507,564.21323 L 415.74891,558.96969 L 420,549.41992 L 420.329,545.35065 L 417.70834,543.36003 L 417.79667,534.77107 z",
    "L 542.47619,512.69264 L 526.68398,512.27706 L 520.24243,507.15151 L 511.30736,502.02597 L 499.87879,501.47186 L 497.45455,505.48918 L 490.8052,505.48918 L 485.67966,503.34199 L 481.24675,496.7619 L 478.68398,497.03896 L 476.46753,500.50216 L 469.33334,503.54978 L 468.64069,506.38961 L 464.62338,509.85282 L 464.13853,512.76191 L 459.98268,512.7619 L 459.77489,519.48052 L 461.71429,521.28138 L 461.71429,529.59307 L 459.91342,531.53247 L 452.57143,532.50216 L 452.29437,530.83983 L 448.9697,529.87013 L 444.67533,529.87013 L 440.1039,534.58009 L 435.94805,534.71861 L 435.4632,536.45022 L 426.73593,536.24242 L 420.98701,535.68831 L 417.66234,534.99567 L 417.69697,543.34199 L 420.41559,545.36796 L 420.08658,549.54112 L 417.33334,555.30736 L 415.82685,558.9264 L 417.78355,564.20779 L 416.33156,585.52293 L 415.58849,629.51631 L 543.25237,643.44281 L 561.20821,606.2407 L 561.7316,571.5671 L 562.26195,560.9602 L 563.47509,556.54987 L 561.86725,540.03549 L 558.82251,536.24242 L 558.82251,531.94805 L 561.59307,527.37662 L 560.91028,525.07903 L 565.40464,521.28834 L 569.08547,517.3897 L 563.12877,516.65156 L 562.10848,514.5974 L 557.77147,517.15398 L 554.80062,518.02325 L 553.81376,515.66677 L 557.04829,511.88812 L 555.35931,506.73593 L 546.63204,506.5974 z",
    "L 658.42424,491.91342 L 652.74459,491.77489 L 651.08225,493.99134 L 650.8052,500.64069 L 646.64935,503.41125 L 641.94924,503.53298 L 638.79671,507.07733 L 628.46448,507.95892 L 626.12391,509.59987 L 625.75179,512.5385 L 610.24718,522.06959 L 609.40905,523.93072 L 601.56576,524.4008 L 595.21146,524.65409 L 589.00962,523.67743 L 588.19575,521.44129 L 585.09363,520.47975 L 584.84817,517.78375 L 568.95883,517.49358 L 565.64112,521.19039 L 560.6738,525.07903 L 561.59307,527.37662 L 558.68398,531.94805 L 558.82251,536.38095 L 561.67134,540.13345 L 563.57305,556.70521 L 562.3599,560.84544 L 561.21229,606.81111 L 561.22741,606.68005 L 590.12987,579.74026 L 602.18182,576.41559 L 602.87446,570.73593 L 603.98269,567.82684 L 607.37662,567.61905 L 608.62338,561.66233 L 611.84417,559.51515 L 615.16883,554.87446 L 617.13524,550.32961 L 621.18672,547.21005 L 624.99578,548.5234 L 632.57923,547.92932 L 638.39647,548.02789 L 642.10114,549.581 L 644.97464,552.13844 L 649.26806,552.72185 L 654.3234,551.1927 L 657.78565,548.10512 L 663.76937,546.6079 L 667.29423,544.86825 L 675.30159,547.4159 L 683.83518,550.88307 L 695.15021,565.17313 L 724.6491,565.13046 L 695.84514,480.29575 z",
    "L 695.15101,565.24834 L 700.56477,576.68657 L 697.71973,584.91303 L 686.93483,589.08304 L 681.55461,592.64502 L 673.43847,598.56199 L 668.30068,598.00275 L 663.33606,595.08855 L 657.87412,586.26043 L 653.58968,584.60114 L 649.44376,585.40072 L 644.95152,588.55529 L 638.03018,592.4809 L 630.5201,592.597 L 621.9018,590.42739 L 616.20779,587.60173 L 609.14285,581.12554 L 607.30736,575.2381 L 607.16883,567.82684 L 603.84416,567.68831 L 602.87446,570.87446 L 602.04329,576.69263 L 589.85282,579.74026 L 561.01997,606.89803 L 543.41895,643.54753 L 552.78099,681.70681 L 573.81856,706.89785 L 647.85055,713.01615 L 727.44446,698.80992 L 740.88647,644.79209 L 724.61693,565.08148 z",
    "L 617.05115,550.47504 L 615.15582,554.83316 L 611.74927,559.54239 L 608.63947,561.63364 L 607.37795,567.55746 L 607.25131,575.52354 L 609.15926,581.22073 L 616.15564,587.61752 L 621.81081,590.47067 L 630.45837,592.58916 L 638.02497,592.45064 L 645.11439,588.55576 L 649.45702,585.33672 L 653.57925,584.59931 L 657.94984,586.35119 L 663.32795,595.21626 C 663.32795,595.21626 668.35699,598.07783 668.35699,598.07783 L 673.47556,598.49761 L 681.76985,592.51426 L 686.96945,589.08118 L 697.80117,584.88116 L 700.53681,576.62337 L 695.18955,565.25915 L 683.76721,550.72629 L 675.03046,547.40227 L 667.24876,544.8374 L 663.74426,546.58508 L 657.76307,548.15367 L 654.24669,551.24259 L 649.19952,552.74119 L 644.95435,552.06065 L 642.01606,549.582 L 638.37549,548.08022 L 632.72308,547.91719 L 624.92493,548.38524 L 621.13077,547.06675 z",
    "L 497.59308,353.80086 L 481.66234,354.07792 L 385.80416,370.15744 L 326.81176,411.95793 L 354.00415,452.83963 L 356.12433,449.18867 L 358.49253,449.47677 L 360.63529,453.37524 L 377.47237,453.15916 L 384.34428,453.53416 L 389.45302,456.39572 L 403.53247,466.83983 L 406.23377,471.75757 L 412.46753,478.19913 L 412.88311,491.5671 L 421.61039,491.70563 L 421.74892,489.76624 L 431.58441,489.62771 L 434.56277,493.36797 L 440.45022,494.12988 L 446.61472,496.48485 L 449.59308,500.77922 L 451.04762,505.76624 L 453.67539,506.92368 L 456.54806,510.72589 L 460.00845,510.65891 L 460.04044,512.74692 L 463.94162,512.72891 L 464.62532,509.93217 L 468.70145,506.31075 L 469.30022,503.61989 L 476.46191,500.43926 L 478.72589,497.06273 L 481.43067,496.77175 L 481.34376,494.96248 L 475.42857,492.46753 L 473.42051,490.20907 L 471.09957,486.71862 L 470.96104,480.62338 L 473.35065,477.64502 L 474.94373,473.76623 L 479.61311,472.9242 L 486.61117,468.15311 L 489.64057,467.93101 L 489.86113,470.65471 L 496.48862,470.67711 L 499.79142,474.54366 L 503.33665,474.46216 L 505.60049,478.70967 L 509.80373,479.18228 L 519.5481,479.3432 L 519.52623,475.00197 L 516.97623,471.45727 L 516.5734,468.91689 L 513.78097,465.71852 L 506.69417,460.54611 L 506.54009,453.78116 L 501.10287,444.25446 L 500.52499,433.89126 L 497.44783,433.29732 L 497.59308,408.31168 z",
    "L 412.45434,478.09436 L 406.54061,471.96297 L 403.37534,466.73863 L 394.16765,459.68593 L 389.46584,456.25753 L 384.17632,453.51482 L 376.92771,453.22096 L 360.56936,453.41686 L 358.80618,449.40074 L 356.16142,449.20483 L 353.92036,452.92709 L 326.87313,411.98224 L 243.65219,475.1056 L 318.68554,517.07028 L 333.534,517.4789 L 335.00332,515.91164 L 343.23147,516.00959 L 348.7169,518.85026 L 365.36912,518.94822 L 365.66298,520.71139 L 372.32386,520.9073 L 373.51612,522.21432 L 374.70346,522.07579 L 375.26249,520.80934 L 385.71982,520.2167 L 389.80971,517.39343 L 394.31518,512.97709 L 397.13207,509.10034 L 402.94499,502.34473 L 409.20709,497.34584 L 412.92815,491.4214 z",
    "L 497.52381,385.87012 L 497.31602,433.38528 L 500.50217,433.87013 L 501.19481,444.05194 L 506.59741,453.74892 L 506.73594,460.53679 L 513.73927,465.6626 L 516.54638,468.89212 L 516.96245,471.54364 L 519.49106,474.95708 L 519.52617,479.54801 L 519.29757,485.57887 L 522.68513,487.44333 L 540.71684,487.39769 L 543.89051,486.15093 L 546.10288,484.75605 L 549.63908,484.82328 L 549.81818,479.86147 L 559.65368,479.44589 L 559.65368,475.98268 L 563.04762,475.08225 L 570.25108,474.52813 L 570.94373,468.64069 L 577.38528,468.64069 L 576.69264,463.65368 L 581.95671,460.74459 L 584.72727,462.68398 L 592.90043,462.54545 L 593.87013,458.66667 L 634.45888,458.66667 L 634.8052,443.84415 L 641.59308,443.63636 L 642.00866,386.07792 L 593.67422,354.19269 L 497.42571,353.74348 z",
    "L 434.6833,493.4155 L 431.5591,489.5666 L 421.74892,489.6277 L 421.61039,491.5671 L 412.81385,491.42857 L 409.0736,497.24675 L 402.90909,502.37229 L 397.02164,509.2987 L 394.11255,513.17749 L 389.74892,517.4026 L 385.76039,520.14744 L 385.84527,526.88454 L 387.80724,529.60553 L 401.6142,530.50597 L 415.65119,531.25887 L 417.99501,534.96372 L 420.71323,535.60043 L 426.6942,536.20207 L 435.4797,536.57301 L 435.99408,534.62088 L 440.10816,534.42498 L 444.71201,529.82113 L 448.92404,529.82113 L 452.25448,530.89863 L 452.6463,532.26998 L 459.699,531.48635 L 461.75603,529.42931 L 461.65808,521.39707 L 459.699,519.34003 L 459.97606,512.71769 L 460.02529,510.62296 L 456.67887,510.60734 L 453.57677,506.7564 L 451.0751,505.99281 L 449.65176,501.16809 L 446.77137,496.50482 L 440.60266,494.0864 z",
    "L 642.15663,386.15103 L 641.52833,443.72348 L 634.72906,443.72601 L 634.52127,458.70639 L 593.70032,458.70639 L 593.01464,462.72251 L 584.59058,462.72251 L 582.06058,460.90196 L 576.75425,463.70205 L 577.4182,468.73745 L 570.85816,468.70876 L 570.34208,474.64423 L 562.87752,475.15777 L 559.71022,475.84837 L 559.61227,479.57063 L 549.71889,479.86449 L 549.64963,484.92246 L 550.60048,486.42743 L 550.60048,492.01081 L 549.71889,492.79445 L 549.71889,495.83103 L 546.68231,499.16147 L 546.58436,506.70394 L 555.49819,506.70394 L 556.86955,511.89551 L 553.82109,515.68704 L 554.75778,517.92127 L 557.82502,517.04701 L 562.09501,514.5126 L 563.04066,516.49936 L 569.01587,517.33443 L 584.91316,517.84232 L 585.012,520.48302 L 588.11694,521.49503 L 589.09648,523.94388 L 595.16964,524.53161 L 600.85099,524.43364 L 609.373,523.94388 L 610.05867,521.9848 L 618.18887,516.89118 L 625.63338,512.38529 L 626.22112,509.54461 L 628.66996,508.07531 L 638.56334,507.19371 L 641.99174,503.56941 L 646.59558,503.47145 L 650.80761,500.53283 L 651.10148,493.87194 L 652.66874,491.81491 L 658.53021,491.88417 L 695.82532,480.31081 L 677.03249,424.56931 z",
    "L 519.29515,485.58511 L 519.52617,479.54801 L 509.69114,479.18656 L 505.57186,478.7797 L 503.42658,474.56332 L 499.65934,474.56359 L 496.37696,470.65044 L 489.78598,470.68855 L 489.70965,467.82033 L 486.61957,468.09573 L 479.68237,472.87522 L 474.97836,473.80086 L 473.07359,477.78355 L 470.92641,480.65801 L 471.1342,486.64935 L 473.37993,490.21746 L 473.4716,490.31815 L 475.42857,492.46753 L 477.67966,493.4026 L 481.41992,494.96104 L 481.52381,496.83116 L 485.81818,503.54978 L 490.8052,505.35065 L 497.31602,505.35065 L 499.80953,501.47186 L 511.30736,502.02597 L 520.17316,507.01299 L 520.17316,507.01299 L 526.68398,512.13853 L 542.19914,512.55411 L 546.63204,506.5974 L 546.77056,499.25541 L 549.67965,495.65368 L 549.67965,492.88312 L 550.64935,491.63636 L 550.51082,486.37229 L 549.59011,484.92123 L 545.80206,484.85713 L 543.63817,486.25371 L 540.65057,487.4923 L 522.69277,487.41222 z",
};

// STATIC: 0x00ADEEB8
float CWorld::detailDoodadDist = 70.0f;

// STATIC: 0x00ADEEC4
int32_t World::lodMax = 3;

// STATIC: 0x00ADEEC8
int32_t World::lodMin = 2;

// STATIC: 0x00ADEF58
static Zone s_zones[11] = {
    { 571, -1, 914, ZonePath(s_zonePaths[0]) },
    { 571, -1, 825, ZonePath(s_zonePaths[1]) },
    { 571, -1, 959, ZonePath(s_zonePaths[2]) },
    { 571, -1, 862, ZonePath(s_zonePaths[3]) },
    { 571, -1, 1847, ZonePath(s_zonePaths[4]) },
    { 571, -1, 1703, ZonePath(s_zonePaths[5]) },
    { 571, -1, 1796, ZonePath(s_zonePaths[6]) },
    { 571, -1, 1777, ZonePath(s_zonePaths[7]) },
    { 571, -1, 1792, ZonePath(s_zonePaths[8]) },
    { 571, -1, 1589, ZonePath(s_zonePaths[9]) },
    { 571, -1, 1740, ZonePath(s_zonePaths[10]) },
};

// STATIC: 0x00ADF168
C44Matrix World::idMat;

// STATIC: 0x00ADF1A8
TSExplicitList<SWModelFadeout, offsetof(SWModelFadeout, m_link)> s_fadeoutList;

// STATIC: 0x00ADF1B4
TSExplicitList<SWModelFadeout, offsetof(SWModelFadeout, m_link)> s_fadeoutDataPool;

// STATIC: 0x00CB753C
int32_t CWorld::bShowSimpleDoodads;

// STATIC: 0x00CD7540
int32_t CWorld::bLoadSimpleDoodads;

// STATIC: 0x00CD7544
Weather* World::s_weather;

// STATIC: 0x00CD7548
Particulate* CWorld::particulate;

// STATIC: 0x00CD754C
CM2Scene* World::s_m2Scene;

// STATIC: 0x00CD7550
int32_t CWorld::shadowMipLevel;

// STATIC: 0x00CD7554
CTexture* CWorld::shadowModTexture;

// STATIC: 0x00CD7658
int32_t CWorld::unk_CD7658;

// STATIC: 0x00CD765C
int32_t CWorld::terrainAlphaBitDepth;

// STATIC: 0x00CD7660
uint32_t CWorld::maxLights;

// STATIC: 0x00CD7664
int32_t CWorld::enableLayerCnt;

// STATIC: 0x00CD7668
float CWorld::farFog;

// STATIC: 0x00CD766C
int32_t CWorld::detailDoodadAlphaRef;

// STATIC: 0x00CD7670
int32_t CWorld::detailDoodadTest;

// STATIC: 0x00CD7674
float World::groundEffectDistValueSqr;

// STATIC: 0x00CD7678
WorldStatus CWorld::status;

// STATIC: 0x00CD768C
int32_t CWorld::chunkCnt;

// STATIC: 0x00CD7690
int32_t CWorld::frameCnt;

// STATIC: 0x00CD7694
int32_t CWorld::s_elapsedTicks;

// STATIC: 0x00CD769C
float CWorld::s_elapsedTimeSec;

// STATIC: 0x00CD774C
uint32_t CWorld::enables;

// STATIC: 0x00CD7750
uint32_t CWorld::enables2;

// STATIC: 0x00CD7754
uint32_t CWorld::m_M2PassMask;

// STATIC: 0x00CD7784
CAaBox CWorld::s_groupAreaOfInterest;

// STATIC: 0x00CD779C
CAaBox CWorld::s_objectAreaOfInterest;

// STATIC: 0x00CD77B8
static TSGrowableArray<CM2Model*> s_unk_CD77B8;

// STATIC: 0x00CD77C8
CiRect CWorld::chunkRect;

// STATIC: 0x00CD77D8
CiRect CWorld::gbChunkRect;

// STATIC: 0x00CD77E8
CiRect CWorld::gbChunkRectOld;

// STATIC: 0x00CD77F8
C4Vector World::texVect[8];

// STATIC: 0x00CD7878
uint32_t CWorld::shadowColor;

// STATIC: 0x00ADEECC
float World::s_horizonFarClipScale = 1.0f;

// STATIC: 0x00ADEED0
float World::s_horizonNearClipScale = 0.7f;

// STATIC: 0x00ADEED4
float World::s_nearClip = 0.1f;

// STATIC: 0x00CD7734
static CM2Model* s_unk_CD7734;

// STATIC: 0x00CD7738
static CM2Model* s_unk_CD7738;

// STATIC: 0x00CD773C
int32_t World::s_detailDoodadDensity;

// STATIC: 0x00CD7744
float World::s_prevFarClip;

// STATIC: 0x00CD7748
float World::s_farClip;

// STATIC: 0x00ADEEE0
int32_t World::s_texCacheDirty = 1;

// STATIC: 0x00CD7760
uint32_t World::s_texCacheOverride;

// STATIC: 0x00CDFFD0
int32_t World::s_waterLOD;

// STATIC: 0x00CD8790
float s_viewerLiquidDepth;


// STATIC: 0x00CD8798
int32_t CWorldScene::frustumIndex;

// STATIC: 0x00CDB108
C3Vector CWorldScene::s_frustumCorners[8];

// STATIC: 0x00CDB168
CWFrustum CWorldScene::frustumStack[32];

// STATIC: 0x00CD8F90
C4Plane CWorldScene::camPlaneXY;

// STATIC: 0x00CD9048
CSortTable CWorldScene::sortTable;

// STATIC: 0x00CDFFF4
CALLBACK_FUNC World::s_loadProgressCallback;

// STATIC: 0x00CDFFF0
void* World::s_loadProgressParam;

// VERIFY: 8564474b manual
// OFFSET: 0x005EEB70
void World::SetTexLodBias(float bias) {
}

// VERIFY: 64ab1743 auto
// OFFSET: 0x0077EC80
bool World::MapIsDungeon() {
    return CMap::bDungeon != 0;
}

// VERIFY: e5c4a380 manual
// OFFSET: 0x0077ED40
static void ZonesInitialize() {
    for (uint32_t i = 0; i < sizeof(s_zones) / sizeof(s_zones[0]); i++) {
        ZonePath& zonePath = s_zones[i].m_zonePath;

        if (zonePath.m_path) {
            for (const char* c = zonePath.m_path; *c != 'z'; c++) {
                if (*c == 'M' || *c == 'L') {
                    zonePath.m_pointCount++;
                }
            }

            zonePath.m_points = static_cast<C2Vector*>(SMemAlloc(sizeof(C3Vector) * zonePath.m_pointCount, __FILE__, __LINE__, 0));

            C2Vector* point = zonePath.m_points;

            for (const char* c = zonePath.m_path; *c != 'z'; c++) {
                if (*c == 'M' || *c == 'L') {
                    const char* x = c + 2;

                    while (*c != ',') {
                        c++;
                    }

                    c++;
                    const char* y = c;

                    while (*c != ' ') {
                        c++;
                    }

                    point->x = atof(x);
                    point->y = atof(y);
                    point++;
                }
            }
        }

        C2Vector* point = zonePath.m_points;

        for (int32_t j = 0; j < zonePath.m_pointCount; j++, point++) {
            point->x += zonePath.m_offsetX;
            float y = (point->y + zonePath.m_offsetY) / 1024.0f;
            point->x *= 33.333332f;
            point->y = y * 34133.332f;

            if (point->x < zonePath.m_minX) {
                zonePath.m_minX = point->x;
            }

            if (zonePath.m_minY > point->y) {
                zonePath.m_minY = point->y;
            }

            if (zonePath.m_maxX < point->x) {
                zonePath.m_maxX = point->x;
            }

            if (zonePath.m_maxY < point->y) {
                zonePath.m_maxY = point->y;
            }
        }

        zonePath.m_minX -= 50.0f;
        zonePath.m_minY -= 50.0f;
        zonePath.m_maxX += 50.0f;
        zonePath.m_maxY += 50.0f;
    }
}

// VERIFY: 6886f483 auto
// OFFSET: 0x0077F1E0
int32_t World::QueryObjectLiquid(CMapEntity* entity, int32_t* liquidType, float* height, int32_t* a4) {
    if (!entity || !(entity->unk_007C & 0x20)) {
        return 0;
    }

    *height = entity->unk_0080;
    *liquidType = entity->unk_00BC;
    *a4 = 0;

    return 1;
}

// VERIFY: 52533b77 auto
// OFFSET: 0x0077F220
int32_t World::QueryObjectLiquidFlags(CMapEntity* entity, int32_t* flag8, int32_t* flag9) {
    if (!entity || !(entity->unk_007C & 0x20)) {
        return 0;
    }

    *flag8 = (entity->unk_007C >> 8) & 1;
    *flag9 = (entity->unk_007C >> 9) & 1;

    return 1;
}

// VERIFY: 305f137e auto
// OFFSET: 0x0077F360
int32_t World::QueryLiquidStatus(C3Vector* pos, int32_t* liquidType, float* height) {
    int32_t a4 = 0;
    return CMap::QueryLiquidStatus(pos, liquidType, height, &a4, 0);
}

// VERIFY: d8d97f86 auto
// OFFSET: 0x0077F390
int32_t World::QueryLiquidSounds(CMapEntity* entity, float radius, uint32_t* results) {
    if (entity->unk_007C & 0x1) {
        uint32_t distance = 9999;
        entity->QueryLiquidSounds(results, &distance);

        if (distance < 2) {
            CMap::QueryLiquidSounds(&entity->m_position, radius, results);
        }

        return 1;
    }

    CMap::QueryLiquidSounds(&entity->m_position, radius, results);

    return 1;
}

// VERIFY: baf0a035 auto
// OFFSET: 0x0077F400
void World::CreateWaterRipple(const C3Vector& pos, float a2, float a3, float a4, float a5, float a6, int32_t a7, int32_t a8) {
    CMap::CreateWaterRipple(pos, a2, a3, a4, a5, a6, a7, a8);
}

// VERIFY: e146f1dc auto
// OFFSET: 0x0077F490
void World::SetNearClip(float nearClip) {
    World::s_nearClip = 0.2f;
}

// VERIFY: 632775d6 auto
// OFFSET: 0x0077F4A0
void World::SetHorizonFarClipScale(float scale) {
    World::s_horizonFarClipScale = scale;
}

// VERIFY: 28a08d04 auto
// OFFSET: 0x0077F4B0
void World::SetHorizonNearClipScale(float scale) {
    World::s_horizonNearClipScale = scale;
}

// VERIFY: 7db3332e auto
// OFFSET: 0x00780620
int32_t World::SceneCamLiquidStatus(float* depth) {
    if (depth) {
        *depth = s_viewerLiquidDepth;
    }

    return s_liquidTypeId;
}

// VERIFY: fa04a252 auto
// OFFSET: 0x00780640
C3Vector* World::GetCamPos() {
    return &CWorldScene::s_activeWorldView;
}

// VERIFY: 21d7eb14 auto
// OFFSET: 0x00780650
C3Vector* CWorldScene::GetCamTarget() {
    return &CWorldScene::camTarget;
}

// VERIFY: bf7ac74d auto
// OFFSET: 0x00780710
void World::SetDetailDoodadDensity(int32_t density) {
    if (World::s_detailDoodadDensity != density) {
        World::s_detailDoodadDensity = density;
        CDetailDoodad::s_bPoolsDirty = 1;
    }
}

// VERIFY: 542dd26b auto
// OFFSET: 0x00780730
void World::SetDetailDoodadDist(float dist) {
    if (dist != CWorld::detailDoodadDist) {
        CWorld::detailDoodadDist = dist;
        CDetailDoodad::s_bPoolsDirty = 1;
        World::groundEffectDistValueSqr = dist * dist;
    }
}

// VERIFY: 5f5281b3 auto
// OFFSET: 0x00780770
float World::ValidateFarClip(float farClip, int32_t mapID) {
    float maxFarClip = 4750.0f / 3.0f;

    if (mapID < 530 || mapID == 575 || mapID == 543) {
        if (!CWorldParam::cvar_farClipOverride || CWorldParam::cvar_farClipOverride->m_intValue < 1) {
            maxFarClip = 2375.0f / 3.0f;
        }
    } else if (OsGetPhysicalMemory() <= 0x40000000) {
        maxFarClip = 2375.0f / 3.0f;
    }

    if (farClip < 550.0f / 3.0f) {
        return 550.0f / 3.0f;
    }

    if (farClip <= maxFarClip) {
        return farClip;
    }

    return maxFarClip;
}

// VERIFY: 1f595043 auto
// OFFSET: 0x00780800
void World::SetFarClip(float farClip) {
    float validated = World::ValidateFarClip(farClip, s_mapId);

    if (World::s_farClip != validated) {
        World::s_prevFarClip = World::s_farClip;
        World::s_farClip = validated;
        CMapRenderChunk::SetDirty();
        World::s_nearClip = 0.2f;
        CMapObj::bPoolsDirty = 1;
        World::s_texCacheDirty = 1;
    }
}

// VERIFY: f80e8676 manual
// OFFSET: 0x00782E40
void CWorldScene::ClearObjectFadeoutList() {
    SWModelFadeout* fadeout = s_fadeoutList.Head();

    while (!s_fadeoutList.IsEnd(fadeout)) {
        SWModelFadeout* next = s_fadeoutList.RawNext(fadeout);

        World::ObjectDestroy(fadeout->m_entity);
        fadeout->m_entity = nullptr;
        fadeout->m_link.Unlink();
        s_fadeoutDataPool.LinkToTail(fadeout);

        fadeout = next;
    }
}

// VERIFY: 1eb0b466 manual
// OFFSET: 0x007837F0
void CWorld::Destroy() {
    CWorldScene::ClearObjectFadeoutList();
    s_fadeoutDataPool.Clear();
    CWorldParam::Destroy();
    CMap::Destroy();
    CWorldScene::Destroy();

    if (CWorld::shadowModTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(CWorld::shadowModTexture));
    }

    CWorld::shadowModTexture = nullptr;

    if (g_BspDigestCache) {
        CAaBspDigestCache* cache = g_BspDigestCache;
        cache->Reset();
        SMemFree(cache, "delete", SLOG_FUNCTION, 0);
    }

    g_BspDigestCache = nullptr;

    if (CWorld::particulate) {
        Particulate* particulate = CWorld::particulate;
        particulate->~Particulate();
        SMemFree(particulate, "delete", SLOG_FUNCTION, 0);
    }

    if (World::s_weather) {
        Weather* weather = World::s_weather;
        weather->~Weather();
        SMemFree(weather, "delete", SLOG_FUNCTION, 0);
    }

    for (uint32_t i = 0; i < s_unk_CD77B8.Count(); i++) {
        if (s_unk_CD77B8[i]) {
            s_unk_CD77B8[i]->Release();
        }
    }

    s_unk_CD77B8.SetCount(0);

    if (s_unk_CD7734) {
        s_unk_CD7734->Release();
    }

    if (s_unk_CD7738) {
        s_unk_CD7738->Release();
    }

    if (World::s_m2Scene) {
        World::s_m2Scene->Release();
        World::s_m2Scene = nullptr;
    }

    CShadowCache::Destroy();
    CShadowQuery::FreeBuffers();
}

// VERIFY: 7ba61373 manual
// OFFSET: 0x00780F50
void CWorld::Initialize() {
    CWorld::enables |= 0x7104B73;
    CWorld::s_elapsedTimeSec = 0.0f;
    CWorld::frameCnt = 0;
    CWorld::chunkCnt = 0;
    CWorld::s_elapsedTicks = 0;

    if (g_theGxDevicePtr->Caps().m_shaderTargets[4] > 0) {
        CWorld::enables |= 0x10000000;
    }

    if (g_theGxDevicePtr->Caps().m_shaderTargets[0] > 0) {
        CWorld::enables2 |= 0x1;
    }

    CWorld::m_M2PassMask |= 0x7;
    CWorld::terrainAlphaBitDepth = CWorldParam::cvar_terrainAlphaBitDepth->m_intValue;
    CWorld::shadowMipLevel = CWorldParam::cvar_shadowLevel->m_intValue;
    CWorld::farFog = CWorldParam::cvar_farClip->m_floatValue;
    CWorld::enableLayerCnt = 4;
    CWorld::status.m_chunksChanged = 0;
    CWorld::status.m_prepareAll = 0;
    CWorld::bShowSimpleDoodads = 0;
    CWorld::bLoadSimpleDoodads = 0;
    World::s_m2Scene = M2CreateScene();

    for (uint32_t i = 0; i < 8; i++) {
        World::texVect[i].x = 0.0f;
        World::texVect[i].y = 0.0f;
        World::texVect[i].z = 0.0f;
        World::texVect[i].w = 1.0f;
    }

    World::idMat.Identity();
    CWorld::unk_CD7658 = 1;
    CWorld::shadowColor = 0xFFFFFFFF;
    CWorld::detailDoodadAlphaRef = 128;
    CWorld::detailDoodadTest = 0;
    World::groundEffectDistValueSqr = CWorld::detailDoodadDist * CWorld::detailDoodadDist;
    CWorld::shadowMipLevel = CWorldParam::cvar_shadowLevel->m_intValue;

    CShaderEffect::InitShaderSystem(M2GetCacheFlags() & 0x8, CWorld::enables2 & 0x2);
    CShaderEffectManager::AddEffectFile("MapObj.wfx");
    CShaderEffectManager::AddEffectFile("MapObjU.wfx");
    CShaderEffectManager::AddEffectFile("Model2.wfx");
    CShaderEffectManager::AddEffectFile("Particle.wfx");
    CShaderEffectManager::AddEffectFile("ShadowMap.wfx");
    CShadowQuery::Initialize();
    CWorldScene::Initialize();
    CMap::Initialize();

    CWorld::chunkRect = CiRect();
    CWorld::gbChunkRect = CiRect();
    CWorld::gbChunkRectOld = CiRect();

    CWorld::s_groupAreaOfInterest.b.x = 0.0f;
    CWorld::s_groupAreaOfInterest.b.y = 0.0f;
    CWorld::s_groupAreaOfInterest.b.z = 0.0f;
    CWorld::s_groupAreaOfInterest.t.x = 0.0f;
    CWorld::s_groupAreaOfInterest.t.y = 0.0f;
    CWorld::s_groupAreaOfInterest.t.z = 0.0f;
    CWorld::s_objectAreaOfInterest.b.x = 0.0f;
    CWorld::s_objectAreaOfInterest.b.y = 0.0f;
    CWorld::s_objectAreaOfInterest.b.z = 0.0f;
    CWorld::s_objectAreaOfInterest.t.x = 0.0f;
    CWorld::s_objectAreaOfInterest.t.y = 0.0f;
    CWorld::s_objectAreaOfInterest.t.z = 0.0f;

    World::lodMax = 3;
    World::lodMin = 2;

    CWorld::particulate = new (SMemAlloc(sizeof(Particulate), __FILE__, __LINE__, 0)) Particulate(0.027777778f, 30.0f, "Textures\\WaterPoop02.blp");
    World::s_weather = new (SMemAlloc(sizeof(Weather), __FILE__, __LINE__, 0)) Weather();

    World::s_m2Scene->SetProjectTextureCallback(World::ProjectTextureCallback, nullptr);
    World::s_m2Scene->SetProjectPositionCallback(World::ProjectPositionCallback, nullptr);

    ConsoleCommandRegister("showDetailDoodads", CWorld::ConsoleCommand_ShowDetailDoodads, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("maxLOD", CWorld::ConsoleCommand_MaxLOD, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showCull", CWorld::ConsoleCommand_ShowCull, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("setShadow", CWorld::ConsoleCommand_SetShadow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterRipples", CWorld::ConsoleCommand_WaterRipples, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterParticulates", CWorld::ConsoleCommand_WaterParticulates, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showShadow", CWorld::ConsoleCommand_ShowShadow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showLowDetail", CWorld::ConsoleCommand_ShowLowDetail, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showSimpleDoodads", CWorld::ConsoleCommand_ShowSimpleDoodads, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("detailDoodadAlpha", CWorld::ConsoleCommand_DetailDoodadAlpha, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("characterAmbient", CWorld::ConsoleCommand_CharacterAmbient, CATEGORY_GRAPHICS, nullptr);

    ZonesInitialize();
}

// VERIFY: 0ee74c0d auto
// OFFSET: 0x00406DE0
bool World::IsValidPosition(float x, float y, float z, float a4) {
if (_finite(x) && _finite(y) && _finite(z)) {
    float v4 = -(y - 17066.666f);
    float v5 = -(x - 17066.666f);
    if (a4 > v4)
        return 0;
    if (34133.332f - a4 > v4 && a4 <= v5 && v5 < 34133.332f - a4)
        return 1;
}
return 0;
}




// VERIFY: 91f1484d auto
// OFFSET: 0x0077F8D0
int32_t World::GetFlightBoundsLower(const C3Vector& pos, float* height) {
    return CMap::GetFlightBounds(pos, height, 1);
}




// OFFSET: 0x00783910
bool World::GetFacets(CAaBox* a1, CAaBox* a2, FacetData* a3, uint32_t a4, uint32_t* a5) {
    a3->facets.SetCount(0);
    a3->facetIds.SetCount(0);
    if (a5)
        *a5 = 0;
    if ((a4 & 0x4000) == 0)
        return CMap::GetFacets(a1, a2, a3, a4, a5) != 0;

    static FacetData facetData = FacetData();

    facetData.facets.SetCount(0);
    facetData.facetIds.SetCount(0);
    if (!CMap::GetFacets(a1, a2, &facetData, a4, a5))
        return 0;

    for (int32_t i = 0; i < facetData.facets.Count(); i++) {
        if (facetData.facets[i].plane.n.z >= 0.64278764f) {
            a3->facets.Add(1, &facetData.facets[i]);
            a3->facetIds.Add(1, &facetData.facetIds[i]);
        }
    }
    return 1;
}




// VERIFY: 9d92b389 auto
// OFFSET: 0x0078FB00
void CWorldScene::FrustumXform(C44Matrix& mat) {
    CWorldScene::frustumStack[CWorldScene::frustumIndex].Transform(mat);
}

// VERIFY: ad5c1827 auto
// OFFSET: 0x0078FB20
int32_t CWorldScene::FrustumCull(CAaBox* box) {
    return CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(*box) == 0;
}

// VERIFY: 7ebc59b0 auto
// OFFSET: 0x0078FB50
void CWorldScene::FrustumPop() {
    CWorldScene::frustumIndex--;
}

// OFFSET: 0x00790650
void CWorldScene::GetNearestCornerToCamera(CAaBox* box, C3Vector* outCorner) {
    outCorner->x = (CWorldScene::camTarget.x >= CWorldScene::s_activeWorldView.x)
                       ? box->b.x
                       : box->t.x;

    outCorner->y = (CWorldScene::camTarget.y >= CWorldScene::s_activeWorldView.y)
                       ? box->b.y
                       : box->t.y;

    outCorner->z = (CWorldScene::camTarget.z >= CWorldScene::s_activeWorldView.z)
                       ? box->b.z
                       : box->t.z;
}





// VERIFY: d30349f9 auto
// OFFSET: 0x00790A80
void CWorldScene::RenderLiquidPass() {
    CWorldScene::ApplyFogState();

    if ((CWorld::enables & Enable_Water) && CWorldScene::s_unk_CD8610) {
        CWorldScene::s_unk_CD8610->Flush(CWorldScene::s_activeWorldView, 1);
    }

    CMap::RenderWaterRipples();
}

// VERIFY: d2772b54 manual
// OFFSET: 0x00790AF0
void CWorldScene::FrustumSet(CRect* rect) {
    C3Vector corners[8];

    for (uint32_t i = 0; i < 8; i += 4) {
        C3Vector* src = &CWorldScene::s_frustumCorners[i];

        C3Vector edgeTop(src[2].x - src[1].x, src[2].y - src[1].y, src[2].z - src[1].z);
        C3Vector topLeft(edgeTop.x * rect->minX + src[1].x, edgeTop.y * rect->minX + src[1].y, edgeTop.z * rect->minX + src[1].z);
        C3Vector topRight(edgeTop.x * rect->maxX + src[1].x, edgeTop.y * rect->maxX + src[1].y, edgeTop.z * rect->maxX + src[1].z);

        C3Vector edgeBottom(src[3].x - src[0].x, src[3].y - src[0].y, src[3].z - src[0].z);
        C3Vector bottomLeft(edgeBottom.x * rect->minX + src[0].x, edgeBottom.y * rect->minX + src[0].y, edgeBottom.z * rect->minX + src[0].z);
        C3Vector bottomRight(edgeBottom.x * rect->maxX + src[0].x, edgeBottom.y * rect->maxX + src[0].y, edgeBottom.z * rect->maxX + src[0].z);

        C3Vector left(topLeft.x - bottomLeft.x, topLeft.y - bottomLeft.y, topLeft.z - bottomLeft.z);
        corners[i + 0] = C3Vector(left.x * rect->minY + bottomLeft.x, left.y * rect->minY + bottomLeft.y, left.z * rect->minY + bottomLeft.z);
        corners[i + 1] = C3Vector(left.x * rect->maxY + bottomLeft.x, left.y * rect->maxY + bottomLeft.y, left.z * rect->maxY + bottomLeft.z);

        C3Vector right(topRight.x - bottomRight.x, topRight.y - bottomRight.y, topRight.z - bottomRight.z);
        corners[i + 3] = C3Vector(right.x * rect->minY + bottomRight.x, right.y * rect->minY + bottomRight.y, right.z * rect->minY + bottomRight.z);
        corners[i + 2] = C3Vector(right.x * rect->maxY + bottomRight.x, right.y * rect->maxY + bottomRight.y, right.z * rect->maxY + bottomRight.z);
    }

    CWorldScene::frustumStack[CWorldScene::frustumIndex].CalcPlanesFromCorners(corners);
}

// VERIFY: d9802889 auto
// OFFSET: 0x00791100
void CWorldScene::FrustumSet(CFrustum* frustum) {
    static_cast<CFrustum&>(CWorldScene::frustumStack[CWorldScene::frustumIndex]) = *frustum;
}

// VERIFY: 939d636d auto
// OFFSET: 0x00791120
int32_t CWorldScene::FrustumCull(const C3Vector& center, float radius) {
    CAaSphere sphere;
    sphere.c = center;
    sphere.r = radius;

    return CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(sphere) == 0;
}

// OFFSET: 0x00791640
bool World::NDCXform(CFrustum* frustum, C44Matrix* out, bool includeTranslation) {
    C3Vector* corners = frustum->corners;

    float ax = corners[3].x - corners[0].x;
    float ay = corners[3].y - corners[0].y;
    float az = corners[3].z - corners[0].z;

    float bx = corners[1].x - corners[0].x;
    float by = corners[1].y - corners[0].y;
    float bz = corners[1].z - corners[0].z;

    float cx = corners[4].x - corners[0].x;
    float cy = corners[4].y - corners[0].y;
    float cz = corners[4].z - corners[0].z;

    *out = C44Matrix();

    out->a0 = ax;
    out->a1 = ay;
    out->a2 = az;
    out->b0 = bx;
    out->b1 = by;
    out->b2 = bz;
    out->c0 = cx;
    out->c1 = cy;
    out->c2 = cz;

    if (includeTranslation) {
        out->d0 = corners[0].x;
        out->d1 = corners[0].y;
        out->d2 = corners[0].z;
    }

    float det = out->Determinant();

    if (fabs(det) < 0.00000023841858f) {
        *out = C44Matrix();
        return false;
    }

    *out = out->Inverse(det);

    return true;
}





// VERIFY: 4c456906 auto
// OFFSET: 0x00791950
void CWorldScene::FrustumPush() {
    CWorldScene::frustumIndex++;
    static_cast<CFrustum&>(CWorldScene::frustumStack[CWorldScene::frustumIndex]) = CWorldScene::frustumStack[CWorldScene::frustumIndex - 1];
}

// VERIFY: b58d1116 manual
// OFFSET: 0x00792DF0
void CWorldScene::AddToDistanceBucket(CChunkLiquid* liquid, const C3Vector& pos) {
    int32_t bucket = 0;
    float distance = pos.y * CWorldScene::camPlaneXY.n.y + pos.z * CWorldScene::camPlaneXY.n.z + pos.x * CWorldScene::camPlaneXY.n.x + CWorldScene::camPlaneXY.d;

    if (distance > 0.0f) {
        float scaled = distance * 0.03f;
        bucket = CMath::fint_(scaled - 0.5f);

        if (bucket >= 64) {
            return;
        }
    }

    CWorldScene::sortTable.m_table[bucket].m_liquidList.LinkToTail(liquid);
}

// OFFSET: 0x00792FC0
void CWorldScene::AddViewerGroup(TSGrowableArray<uint16_t>* group, uint16_t val) {
    for (int32_t i = 0; i < group->Count(); i++) {
        if (group->Ptr()[i] == val) {
            return;
        }
    }

    group->Add(1, &val);
}


















// VERIFY: 44ac3c58 auto
// OFFSET: 0x007935A0
void CWorldScene::CullLiquid(CSortEntry* entry) {
    if (!(CWorld::enables & Enable_Water)) {
        CChunkLiquid* liquid = entry->m_liquidList.Head();

        while (liquid) {
            CChunkLiquid* next = entry->m_liquidList.Next(liquid);
            CMap::s_mapChunkLiquidUpdateList.LinkToTail(liquid);
            liquid = next;
        }
    } else {
        CChunkLiquid* liquid = entry->m_liquidList.Head();

        while (liquid) {
            CChunkLiquid* next = entry->m_liquidList.Next(liquid);
            CMap::s_mapChunkLiquidUpdateList.LinkToTail(liquid);

            CAaBox box;
            liquid->GetAaBox(box);

            if (CWorldScene::frustumStack[CWorldScene::frustumIndex].Cull(box) && !CWorldOcclusion::QueryBuffer(&box, 0) && !CWorldOcclusion::QueryVolumes(liquid->GetSphere())) {
                liquid->RenderPrep();

                if (CWorldScene::s_unk_CD8610 && liquid->m_instance) {
                    CWorldScene::s_unk_CD8610->Draw(liquid->m_instance);
                }
            }

            liquid = next;
        }
    }
}

// OFFSET: 0x007A3E40
void World::AddAaBoxFacets(CAaBox* box, FacetData* facets) {
    static const int32_t s_boxTriIndex[36] = { 0, 2, 3, 0, 1, 2, 1, 6, 2, 1, 5, 6, 4, 6, 5, 4, 7, 6, 0, 7, 4, 0, 3, 7, 0, 5, 1, 0, 4, 5, 2, 7, 3, 2, 6, 7 };

    if (box->t.x <= box->b.x || box->t.y <= box->b.y || box->t.z <= box->b.z)
        return;

    uint32_t firstFacet = facets->facets.Count();

    C3Vector corner[8];
    corner[0] = { box->b.x, box->t.y, box->b.z };
    corner[1] = { box->t.x, box->t.y, box->b.z };
    corner[2] = { box->t.x, box->b.y, box->b.z };
    corner[3] = { box->b.x, box->b.y, box->b.z };
    corner[4] = { box->b.x, box->t.y, box->t.z };
    corner[5] = { box->t.x, box->t.y, box->t.z };
    corner[6] = { box->t.x, box->b.y, box->t.z };
    corner[7] = { box->b.x, box->b.y, box->t.z };

    C4Plane plane[6];
    plane[0].From3Pos(corner[0], corner[2], corner[3]);
    plane[1].From3Pos(corner[1], corner[6], corner[2]);
    plane[2].From3Pos(corner[4], corner[6], corner[5]);
    plane[3].From3Pos(corner[0], corner[7], corner[4]);
    plane[4].From3Pos(corner[0], corner[5], corner[1]);
    plane[5].From3Pos(corner[2], corner[7], corner[3]);

    CFacet facet(0.0f);
    for (int32_t i = 0; i < 36; i += 3) {
        facet.plane = plane[i / 6];
        facet.v[0] = corner[s_boxTriIndex[i]];
        facet.v[1] = corner[s_boxTriIndex[i + 1]];
        facet.v[2] = corner[s_boxTriIndex[i + 2]];
        facets->facets.Add(1, &facet);
    }

    uint32_t count = facets->facets.Count();
    facets->facetIds.SetCount(count);
    for (uint32_t i = firstFacet; i < count; i++)
        facets->facetIds[i] = 0;
}

// OFFSET: 0x007A6B90
void CWorldScene::CalcScreenRectFromVerts(CRect& rect, C3Vector* clippedVerts, uint32_t clippedCount) {
    rect.minX = 3.4028235e38;
    rect.maxX = -3.4028235e38;
    rect.maxY = -3.4028235e38;
    rect.minY = 3.4028235e38;

    for (uint32_t i = 0; i < clippedCount; i++) {
        if (rect.minX > clippedVerts[i].x)
            rect.minX = clippedVerts[i].x;
        if (rect.maxX < clippedVerts[i].x)
            rect.maxX = clippedVerts[i].x;
        if (rect.minY > clippedVerts[i].y)
            rect.minY = clippedVerts[i].y;
        if (rect.maxY < clippedVerts[i].y)
            rect.maxY = clippedVerts[i].y;
    }
}














