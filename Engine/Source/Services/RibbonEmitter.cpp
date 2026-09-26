#include "RibbonEmitter.h"

// INLINED
CRibbonMat::CRibbonMat() {
    this->m_lit = 1;
    this->m_fogged = 1;
    this->m_depthTest = 1;
    this->m_depthWrite = 1;
    this->m_cull = 1;
    this->m_blendMode = 0;
}
