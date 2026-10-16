// Joined item worker owns loading; queries copy immutable data.
#pragma once
namespace wxl::native {
void LoadItemVisualGraph() noexcept;
void ClearItemVisualGraph() noexcept;
}
