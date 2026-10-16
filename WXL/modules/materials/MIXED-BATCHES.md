# Mixed native/custom material owners

Native AddAlpha source21/33/35 recipes may now retain ordinary sibling batches in the same SKIN. Previously one stock row caused the complete renderer recipe to be declined. This is an integration coverage change, not a new shader equation.

Preparation marks only the supported AddAlpha rows custom. Every row retains its original24 bytes and the exact existing strict material serializer result; native sibling rows are not rewritten. A private material-only carrier avoids confusing source palettes with the final device-sized topology. Publication requires exact serialized batch identities. Each prepared row is independently serialized and compared against the whole canonical template, establishing a bounded explicit origin for every one-to-one or EnvSplit one-to-two native row. A stock EnvSplit follower remains stock; custom rows must serialize to exactly one native row. Original prepared count and final native count are tracked separately. No row or triangle is dropped.

The normal native conversion still validates the full model, textures and SKIN. Rebuilt topology binds each final batch through the canonical origin map, retains custom/stock ownership, and updates only the validated final section identity. ConvertedMatches now compares all24 native row bytes for both custom and ordinary siblings.

Actual ordinary, projected and doodad dispatch selects the source owner for the current native batch, rather than treating the entire model as custom. Supported stock rows use their existing native equations. Custom rows remain refused on unsupported projected/forced-instanced paths. Whole-model geometry optimization remains disabled for a model containing a custom row so the custom source section/palette identity cannot be merged away. Exact DnD and named Rift owners remain separate.

This source is prepared for the next combined x64 build. Broad gameplay, visual blend ordering, reset and mounted mixed-model acceptance are pending. No DX11 or native fog/liquid/weather implementation changes.
