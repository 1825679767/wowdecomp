# Native creation effect attachment ownership

OwnedVisual holds one native creation reference. AttachToParent adds a separate child attachment reference; DetachFromParent always consumes that reference, including on an already unbound model.

Base-relative effects intentionally remain independent scene roots when the character has no parent. Hand-origin travelling effects can also become scene roots after their first origin capture. Cleanup and hand-to-root transitions now detach only while the actual native m_attachParent is non-null, then cleanup releases its held creation reference exactly once. Previously unconditional detach could delete an unattached effect before the subsequent Release, or leave a travelling effect dangling.

Native detach behavior itself is unchanged. Effect assets, choreography, placements, animation roles, fade and duration policies are unchanged. This source-confirmed reference fix is separate from broad visual and scene/reset gameplay acceptance.
