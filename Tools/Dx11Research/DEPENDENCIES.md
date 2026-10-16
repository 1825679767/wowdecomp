# Native DX11 build dependencies

The source vendors only the MIT-licensed shader-converter portion of Microsoft/D3D9On12, pinned to `f3352d12acb286e09c0b73d44a1fe2dfbb05065e`. Its D3D12 device/mapping layer is not included or linked. WDK build headers and the official DXBC signer come from pinned Microsoft NuGet packages, verified before extraction. Run:

```powershell
python Tools/Dx11Research/fetch_dependencies.py --output E:/WXL1.1/dx11/dependencies
```

Set `WXL_DX11_WDK_ROOT` to `dependencies/wdk/c/Include/10.0.26100.0`, and `WXL_DX11_SIGNER_DLL` to `dependencies/dxbcsigner/build/native/bin/x64/dxbcSigner.dll` when configuring. The signer must be retained next to the AMD64 executable that translates shaders. It is loaded from the executable/system directory. No replacement D3D9 DLL is supplied. Package licenses are retained in the dependency directory.

`UPSTREAM.json` records the unmodified upstream converter inputs. Local changes are listed in `ThirdParty/D3D9ShaderConverter/LOCAL-CHANGES.md`; the final source capture records the actual modified bytes. These dependencies travel with the DX11 integration recipe, independently of installed Win32 runtime families.
