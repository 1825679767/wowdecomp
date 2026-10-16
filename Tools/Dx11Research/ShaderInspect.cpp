#include "Engine/Source/Gx/ShaderContainer/ShaderContainer.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl/client.h>
#endif

int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "usage: ShaderInspect <extracted-v4.bls>\n"; return 2; }
    try {
        const auto size=std::filesystem::file_size(argv[1]);
        if (size>gx::shader::Container::MaxFileBytes) throw std::runtime_error("file exceeds limit");
        std::vector<uint8_t> bytes(static_cast<size_t>(size));
        std::ifstream in(argv[1],std::ios::binary);
        if (!in.read(reinterpret_cast<char*>(bytes.data()),std::streamsize(bytes.size()))) throw std::runtime_error("short read");
        gx::shader::Container container; std::string error;
        if (!container.Open(bytes.data(),bytes.size(),error)) throw std::runtime_error(error);
        std::cout << "permutations=" << container.Count() << " decoded_bytes=" << container.DecodedBytes() << '\n';
        uint32_t failures=0;
        for (uint32_t i=0;i<container.Count();++i) {
            gx::shader::Permutation p;
            if (!container.Extract(i,p,error)) { std::cout << i << " refused=" << std::quoted(error) << '\n'; ++failures; continue; }
            std::cout << i << " bytes=" << p.code.size() << " constant_bytes=" << p.constantBytes
                      << " dxbc_signature=" << gx::shader::HasDxbcSignature(p);
#ifdef _WIN32
            Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflection;
            const HRESULT hr=D3DReflect(p.code.data(),p.code.size(),IID_ID3D11ShaderReflection,
                                       reinterpret_cast<void**>(reflection.GetAddressOf()));
            D3D11_SHADER_DESC desc{};
            if (FAILED(hr) || FAILED(reflection->GetDesc(&desc))) { std::cout << " reflection=refused"; ++failures; }
            else std::cout << " reflection=passed stage=" << D3D11_SHVER_GET_TYPE(desc.Version)
                           << " cbuffer_count=" << desc.ConstantBuffers << " inputs=" << desc.InputParameters;
#endif
            std::cout << '\n';
        }
        return failures ? 1 : 0;
    } catch(const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
