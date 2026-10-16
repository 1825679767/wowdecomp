# Native positioners, timed procedural effects and custom depth passes

Source integration against 5e84f8f88ad2cbe236c181bcaf2426b88891be78. No gameplay/startup/probe/account/pixel tests requested or run. The combined build has its own exact frozen input/output binding.

## Positioner graph and static consumer

The current PTR12.1 Positioner E0D4FD55, PositionerState 9CDFBF30, PositionerStateEntry 855AA6BC and TransformMatrix 45999148 declarations match the primary WoWDBDefs fields and local retained WDC5 header hashes. primary-inputs.json preserves hashes/URLs for the four primary schema snapshots and actual local DB2 inputs; no DB2 data copies or mutations. Runtime reads the current mounted DB2 authority through the existing native item worker.

Owned graph snapshots include raw state-entry types/style/source/destination selectors, signed 16-bit values, curve references, timing flags and transforms. Caps:4096 requested positioners,65536 rows/table,32 states/graph. Missing rows, cycles, malformed/nonfinite transforms and invalid scales refuse atomically. Data graphs are per-catalog, shared immutable through the model recipe and native timeline, never a global last-observed actor.

An unflagged zero-start-life, zero-percent graph with at most one terminal zero-end-life state and no dynamic position/rotation/scale entries can now play its static transform. The existing CEffect model-attachment matrix helper supplies the native signed yaw/Y-offset equation. Explicit compatibility policy composes the positioner locally before the base attachment transform. This is a native-source compatibility port, not proof of arbitrary retail positioner ordering. Empty first-state graphs are identity only. Dynamic entry enums/style/curve/source-destination semantics, nonzero lifetime/percent/flags and multistate timing are preserved in owned data but remain unsupported; no inferred curve/motion equations are enabled. Current refs contain schemas, not an implementation of these newer operations.

## Timed procedural owners

The explicit modern-to-native-compatible payload policy now handles1(packed RGB),13(flash RGB, hold delay, fade duration) and14(alpha with optional fade duration). Values finite and bounded, unused payload fields zero, seconds-to-ms sums bounded by signed clock range. Type13 follows the actual WotLK CreateProceduralEffect/GetFlashColor hold-then-quantized-fade equation. Type14 composes a linear fade from the item's baseline alpha, matching the native fade policy rather than invoking a GUID-bound world unit operation on an item. Numeric compatibility remains explicit, not a claim that all modern subtype enums are native aliases.

Repeated identical visual replay now retains the procedural owner instead of resetting timers. Source payload changes replace the owner. Native clones retain elapsed time on their destination scene clock. Native intervening tint/alpha writers become the rollback baseline, and removal/disable/destruction restores only last owned writes. Existing source selectors and ordered kit rows remain authoritative.

## Custom material depth-only pass

Source inspection corrected an earlier overly broad limitation: M2 element flag1 is the native duplicated depth-only pass, not a different or sorted index buffer. SetupMaterial sets ColorWrite0 and its native blend/depth state. AddAlpha ordinary rigid/weighted GPU and CPU draws now reuse their exact source geometry/pixel coverage in this pass, preserving native masks and normal state restoration. Bit4 remains optimized geometry and is still refused; custom-owned models already retain the unmerged visible geometry route. Projected receiver geometry, forced instancing and OpenGL shader ownership remain unsupported because the donor custom equation does not provide those contracts. No global blend or native renderer table changed.

## Boundaries

No DX11/native world/fog/liquid/water/weather/postprocess changes. No full source/Data copies or Downloads folders. Existing stable client only, exact source5e84f8f rollback kept before local staging. Configuration and Data/Interface links preserved. Gameplay/visual/reset/streaming and authenticated persistence acceptance remain pending. No 100percent parity claim. Primary references are the retained schema URLs; runtime behavior comes from the local native Unit_C.cpp GetFlashColor/CreateProceduralEffect, Effect_C.cpp ApplyModelAttachRecToMatrix and CM2SceneRender.cpp SetupMaterial.
