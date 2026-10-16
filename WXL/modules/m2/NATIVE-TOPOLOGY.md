# Native complete-topology palette adaptation

The source-native ConvertForNativeRuntime path can return owned converted SKIN buffers when a device palette budget requires topology partitioning. The generic/injected-host converters retain their policy. Strict validation still runs on every LOD; only palette/influence refusal triggers a rebuild, and invalid influences still fail inside the partitioner.

Each named LOD is rebuilt from the same owned assembled skeleton and full triangle stream using extended section starts. Native vertices and bone combinations are joined across profiles, with checked uint16 lookup/combo rebasing and a 64MiB model ceiling. Section IDs, triangle order and duplicated source batch order remain intact. Empty/overbudget geometry, unsupported materials and unresolved source records remain refused; no high-start sections are dropped. Every rebuilt SKIN passes the ordinary source and native conversion checks before publication.

The model and all rebuilt SKINs publish together. Original archive companion bytes remain owned and the later native SKIN read must match exactly before replacement. There are no global aliases, prepared filename caches or renderer hooks. Original per-batch custom mesh/donor recipe identities cannot be reused after a topology split, so that unsupported composition remains refused. Joined vertex/combo totals above65535 are explicit limits.

Native compilation is required; gameplay/LOD/device/visual acceptance remains pending. This implementation does not change source geometry until a real palette-budget refusal, nor infer render capability from CPU conversion.
