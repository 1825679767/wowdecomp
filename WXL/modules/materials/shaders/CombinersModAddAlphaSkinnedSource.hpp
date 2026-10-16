// Exact embedded sibling HLSL; source-only generation, no shader compilation.
#pragma once
inline constexpr char kCombinersModAddAlphaSkinnedSource[]=R"WXLVS(
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// Native source variant of the donor AddAlpha VS; D3DCompile vs_3_0 once per native device.

// Native DrawBatch uploads three affine rows per section palette bone at c31.
float4 gBoneRows[225] : register(c31); // Up to75 native palette entries; no instancing.
float4 gProjRow0 : register(c2);  // projection matrix, row 0 (view-space -> clip-space)
float4 gProjRow1 : register(c3);  // projection matrix, row 1
float4 gProjRow2 : register(c4);  // projection matrix, row 2
float4 gProjRow3 : register(c5);  // projection matrix, row 3
float4 gTexGenU0 : register(c6);  // UV0 affine transform, U row {a, b, unused, translate}
float4 gTexGenV0 : register(c7);  // UV0 affine transform, V row
float4 gTexGenU1 : register(c8);  // UV1 affine transform, U row
float4 gTexGenV1 : register(c9);  // UV1 affine transform, V row
float4 gDiffuse  : register(c10); // directional light colour
float4 gAmbient  : register(c11); // ambient colour
float4 gLightDir : register(c12); // light direction, view space
float4 gColorMul : register(c28); // vertex colour multiplier
float4 gColorAdd : register(c29); // vertex colour add
float4 gFogParams: register(c30); // x=density, y=offset, z=pow, w=far

struct VsOut
{
    float4 pos     : POSITION;
    float4 color   : COLOR0;
    float2 uv0     : TEXCOORD0;
    float2 uv1     : TEXCOORD1;
    float  fog     : FOG;
    float  grazing : TEXCOORD2; // 1 = facing the camera, 0 = grazing/silhouette
};

VsOut main(float3 pos : POSITION, float4 weights : BLENDWEIGHT, float4 indices : BLENDINDICES, float3 normal : NORMAL, float2 uv0 : TEXCOORD0, float2 uv1 : TEXCOORD1)
{
    VsOut o;

    // GxVBF_PBNT2: weights UBYTE4N, palette indices UBYTE4. Source
    // admission requires contiguous weights summing255 and real palette entries.
    // Zero-weight lanes select palette0 before addressing, even if their unused
    // source index byte is255. This avoids an out-of-range constant read.
    float4 row0=0,row1=0,row2=0;
    [unroll] for(int lane=0;lane<4;++lane) {
        int base=int(weights[lane]>0?indices[lane]:0)*3;
        row0+=gBoneRows[base]*weights[lane];
        row1+=gBoneRows[base+1]*weights[lane];
        row2+=gBoneRows[base+2]*weights[lane];
    }
    float4 p = float4(pos, 1.0);
    float3 viewPos;
    viewPos.x = dot(row0, p);
    viewPos.y = dot(row1, p);
    viewPos.z = dot(row2, p);

    float4 vp = float4(viewPos, 1.0);
    o.pos.x = dot(gProjRow0, vp);
    o.pos.y = dot(gProjRow1, vp);
    o.pos.z = dot(gProjRow2, vp);
    o.pos.w = dot(gProjRow3, vp);

    o.fog = saturate(pow(max(viewPos.z * gFogParams.x + gFogParams.y, 0.0), gFogParams.z));

    float3 viewNormal;
    viewNormal.x = dot(row0.xyz, normal);
    viewNormal.y = dot(row1.xyz, normal);
    viewNormal.z = dot(row2.xyz, normal);
    viewNormal = normalize(viewNormal);

    float ndotl = saturate(dot(-gLightDir.xyz, viewNormal));
    float3 lit  = saturate(ndotl * gDiffuse.rgb + gAmbient.rgb);
    o.color.rgb = saturate(gColorMul.rgb * lit + gColorAdd.rgb);
    o.color.a   = saturate(gColorMul.a + gColorAdd.a);

    o.uv0.x = gTexGenU0.x * uv0.x + gTexGenU0.y * uv0.y + gTexGenU0.w;
    o.uv0.y = gTexGenV0.x * uv0.x + gTexGenV0.y * uv0.y + gTexGenV0.w;
    o.uv1.x = gTexGenU1.x * uv1.x + gTexGenU1.y * uv1.y + gTexGenU1.w;
    o.uv1.y = gTexGenV1.x * uv1.x + gTexGenV1.y * uv1.y + gTexGenV1.w;

    // The new part: view direction (camera sits at the view-space origin, so this is simply the
    // negated, normalized view-space position) dotted with the view-space normal. High where the
    // surface faces the camera, low toward the mesh's own silhouette -- exactly where a closed blob
    // mesh needs to fade out instead of showing its edge.
    float3 viewDir = normalize(-viewPos);
    o.grazing = saturate(dot(viewNormal, viewDir));

    return o;
}

)WXLVS";
