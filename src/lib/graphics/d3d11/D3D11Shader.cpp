#include "graphics/d3d11/D3D11Shader.hpp"
#include "utils/FileController.hpp"
#include <cassert>

APILearning::D3D11Shader::D3D11Shader(const D3D11Context** context, const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_ShaderGroup(shaderGroup), m_Layout(elements)
{
    HRESULT hr;

    m_DeviceContext = (*context)->GetDeviceContext();

    auto nativeElements = m_Layout.GetElements();
    D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[elements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        ied[i].SemanticName = nativeElements[i].Name.c_str();
        ied[i].SemanticIndex = 0;
        ied[i].Format = GetFormat(nativeElements[i].Type);
        ied[i].InputSlot = 0;
        ied[i].AlignedByteOffset = nativeElements[i].Offset;
        ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        ied[i].InstanceDataStepRate = 0;
    }  

    ID3D11Device* device = (*context)->GetDevice();

    auto details = m_ShaderGroup.GetShaderDetails();
    for (size_t i = 0; i < details.size(); i++)
    {
        BuildShader(m_ShaderGroup.GetBasePath(), details[i].ShaderKind, m_ShaderGroup.GetHLSLVersion(), device);
    }

    device->CreateInputLayout(ied, details.size(), m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), m_InputLayout.GetAddressOf());

    delete[] ied;
}

APILearning::D3D11Shader::~D3D11Shader()
{
    
}

void APILearning::D3D11Shader::Stage() const
{
    m_DeviceContext->IASetInputLayout(m_InputLayout.Get());
    auto details = m_ShaderGroup.GetShaderDetails();
    for (size_t i = 0; i < details.size(); ++i)
    {
        switch (details[i].ShaderKind)
        {
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_VERTEX:
        {
            m_DeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
            break;
        }
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_PIXEL:
        {
            m_DeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
            break;
        }
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_GEOMETRY:
        {
            m_DeviceContext->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
            break;
        }
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_HULL:
        {
            m_DeviceContext->HSSetShader(m_HullShader.Get(), nullptr, 0);
            break;
        }
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_DOMAIN:
        {
            m_DeviceContext->DSSetShader(m_DomainShader.Get(), nullptr, 0);
            break;
        }
        case APILearning::SHADER_KIND::D3D_SHADER_KIND_COMPUTE:
        {
            m_DeviceContext->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
            break;
        }
        default:
            break;
        }
    }
}

uint32_t APILearning::D3D11Shader::GetStride() const
{
    return m_Layout.GetStride();
}

uint32_t APILearning::D3D11Shader::GetOffset() const
{
    return 0;
}

void APILearning::D3D11Shader::BuildShader(std::string_view baseShaderPath, SHADER_KIND shaderKind, HLSL_VERSION hlslVersion, ID3D11Device* device)
{
    HRESULT hr;
    std::string_view filepath = ShaderManager::BuildBlobFilename(shaderKind, hlslVersion, baseShaderPath);
    std::wstring w_Filepath = std::wstring(filepath.begin(), filepath.end());
    switch (shaderKind)
    {
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_VERTEX:
    {
        D3DReadFileToBlob(w_Filepath.c_str(), m_VertexBlob.GetAddressOf());
        hr = device->CreateVertexShader(m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());
        assert(hr == S_OK);
        break; 
    }
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_PIXEL:
    {
        D3DReadFileToBlob(w_Filepath.c_str(), m_PixelBlob.GetAddressOf());
        hr = device->CreatePixelShader(m_PixelBlob->GetBufferPointer(), m_PixelBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf());
        assert(hr == S_OK);
        break; 
    }
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_GEOMETRY: 
    {
        D3DReadFileToBlob(w_Filepath.c_str(), m_GeometryBlob.GetAddressOf());
        hr = device->CreateGeometryShader(m_GeometryBlob->GetBufferPointer(), m_GeometryBlob->GetBufferSize(), nullptr, m_GeometryShader.GetAddressOf());
        assert(hr == S_OK);
        break; 
    }    
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_HULL:
    {
        D3DReadFileToBlob(w_Filepath.c_str(), m_HullBlob.GetAddressOf());
        hr = device->CreateHullShader(m_HullBlob->GetBufferPointer(), m_HullBlob->GetBufferSize(), nullptr, m_HullShader.GetAddressOf());
        assert(hr == S_OK);
        break;
    }
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_DOMAIN:
    {
        D3DReadFileToBlob(w_Filepath.c_str(), m_DomainBlob.GetAddressOf());
        hr = device->CreateDomainShader(m_DomainBlob->GetBufferPointer(), m_DomainBlob->GetBufferSize(), nullptr, m_DomainShader.GetAddressOf());
        assert(hr == S_OK);
        break;
    }
    case APILearning::SHADER_KIND::D3D_SHADER_KIND_COMPUTE:
    {
        D3DReadFileToBlob(w_Filepath.data(), m_ComputeBlob.GetAddressOf());
        hr = device->CreateComputeShader(m_ComputeBlob->GetBufferPointer(), m_ComputeBlob->GetBufferSize(), nullptr, m_ComputeShader.GetAddressOf());
        assert(hr == S_OK);
        break;
    }
    default:
        break;
    }
}

DXGI_FORMAT APILearning::D3D11Shader::GetFormat(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
        case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
        case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
        case ShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
        case ShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
        case ShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
        case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

