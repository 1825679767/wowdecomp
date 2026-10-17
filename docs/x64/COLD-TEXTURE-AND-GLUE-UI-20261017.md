# Cold textures and Glue graphics controls

New user screenshots on source3a0e607 show this port is not at x32 visual parity: black eyes/actors, incorrectly composed gear and residual creation UI. Treat this as failed visual evidence, not source-contract acceptance.

Concrete cold-start defect: EquipmentAttachments::BindTexture created a native asynchronous file texture, checked TextureGetGxTex with wait0, then released it on a miss. The last reference could retire the read before completion; the next retry repeated the same miss. BindTexture now finishes the native read (wait1), then checks final status and transfers the owned reference. Existing model-loading/remap/material validation stays intact. This matches donor RetailEquipment.cpp SafeLoadResource/BindModelMaterials semantics rather than changing DBC precedence.

Whole eye/hair/decoration textures now use native TextureCreate/TextureGetGxTex file loading and cache/alpha/mip ownership, as in donor CharacterGeosets.cpp; only the composed body remains a procedural atlas. Procedural callback disabling applies only to generated sheets, never to shared file textures.

Previous FPS menu was an in-game addon, so it did not solve Glue graphics. The canonical VideoOptionsPanels.lua loader now appends independent controls that work in Glue or FrameXML. Existing early Apply repair remains exact-fingerprint bound. Frame rate button opens separate foreground/background cycling presets Unlimited/60/90/120/144/165/200/240/360. No CVar changes on load. The loose addon detects an already installed native control. Live maxFPS0/maxFPSBk0/gxVSync0 remain untouched.

Native WXL logging had no opened sink. RegisterFeatures now opens Logs/WXL-native.log; bounded image failures, actual recipe identity/attempts, and base atlas nonblack/rgb summaries make a remaining CPU composition vs native loader/render failure distinguishable on the real user run. No credentials/account packets, screenshot dumps or large texture copies are recorded.

Remaining unresolved: actual race/eye/gear pixels on this source, Earthen/Harranir material/layout parity, Demon Hunter/class roster UI/data policy, source/data parity across the mounted client archives, and broader world/save/uncapped memory behavior. No unsupported class removal, guessed race texture, global material replacement or claimed1:1 acceptance.
