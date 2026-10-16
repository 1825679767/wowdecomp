# Native WXL CVar ownership

WXL-owned controls use ordinary native registered/archive CVars (registration flags0; CVar::Register supplies archive bit1). They do not set the native engine's latched bit2.

The original native port registered these controls with2. CVar::Set invokes its callback first and then stores only m_latchedValue when bit2 is set, leaving m_stringValue/m_intValue/m_floatValue unchanged until an explicit CVar::Update owner applies it. WXL had no such apply owner. Callback-based modules therefore reacted while pointer/int-based modules and GetCVar retained the old value, and the toggle appeared ineffective or inconsistent until restart.

The correction changes22 WXL registration sites (including the19 grass parameters and15 core feature toggles registered by loops), not global CVar behavior or native graphics settings. Existing callbacks, compile-time gates, feature dependencies, archive persistence, defaults and rejected-value checks remain. Native SetCVar and console commands now update both their actual value and their normal WXL callback through the same native setter. Controls that require asset/model/world reload still do: changing a decoder or material recipe setting cannot retroactively replace an already loaded source/model/texture. No automatic reload, device reset, account/config edit or server action occurs during installation.

The engine-owned flags and native Set/InternalSet/Update/IterateForArchive behavior are documented directly in Engine/Source/ConsoleBase/ConsoleVar.cpp; Script_SetCVar in WoW/Source/Ui/GameUI_Scripts.cpp calls Set without applying the latched value. This is a source integration correction, not a runtime test result.
