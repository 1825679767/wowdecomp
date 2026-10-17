# Repository instructions

## 构建要求

- 编译时**必须**开启 `WOWDECOMP_DISABLE_SIGNATURE_CHECKS` 选项，以禁用 MPQ 文件签名验证。
  - 这允许客户端加载第三方/私服补丁而不进行签名检查。
- 全新 CMake 配置默认开启 `WOWDECOMP_DISABLE_ITEM_CACHE`，使物品资料使用服务器查询结果，并且不读写持久化的物品 WDB 缓存。
  - 编译脚本或命令应显式传入 `-DWOWDECOMP_DISABLE_ITEM_CACHE=ON`，避免已有 CMake 缓存保留旧的关闭值。

## 发布规则

- 发布构建时，将完整的可发布文件直接同步到 `release` 目录。
- 不要生成或更新 ZIP、压缩包或其他归档发布物，除非用户明确要求。
- `Data` 目录不属于发布构建内容，由用户自行准备。
