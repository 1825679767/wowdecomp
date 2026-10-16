# Native grass shader fixtures

The two GXSH files are exact copies of the six-permutation `DetailDoodad.bls`
families from `dx11/research/stock-shader-corpus-r2/members/shaders/vertex/`
profiles `vs_2_0` and `vs_3_0`. They retain the native source bytes. Their
identities are captured with each immutable source/build snapshot.

`GrassWindChecks.h` checks ordinary shader identity, donor injection, real GPU
creation and reset, unchanged original bytecode, stock directional/point-light
pixels with motion disabled, and actual displaced geometry with motion enabled.
The motion constants occupy c32..39, preserving native point lights c14..22.
Inline trig constants use c40..42. SM2 saturation and sincos use its legal native
forms; the coefficients were obtained from the installed Microsoft vs_2_0
compiler, with the original failing assembly retained in integration evidence.

These checks do not grant in-world visual or performance acceptance.
