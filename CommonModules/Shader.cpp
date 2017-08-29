/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Shader.h>
#include <Matrix4x4.h>
#include <Utils/AutoCOM.h>
#include <Utils/ToString.h>
#include <MathHelpers.h>
#include <Utils/Algorithm.h>
#include <Utils/DirectX.h>
#include <algorithm>

namespace Shaders
{

static ID3D10Blob* CompileShader(const std::wstring &FileName, const std::string &EntryPoint, const std::string &Profile) throw (Exception)
{
	ID3D10Blob* errorsMsg = NULL;
	ID3D10Blob* shaderBuffer = NULL;

	HRESULT hr = D3DX11CompileFromFile(
		FileName.c_str(),
		NULL,
		NULL,
		EntryPoint.c_str(),
		Profile.c_str(),
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		NULL,
		&shaderBuffer,
		&errorsMsg,
		NULL);

    if(errorsMsg){
        std::string message(reinterpret_cast<char*>(errorsMsg->GetBufferPointer()));
        ReleaseCOM(errorsMsg);
		throw ShaderCompilationException(message);
    }

    HR(hr);        

    return shaderBuffer; 
}

void PixelShader::Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> shaderBuffer = CompileShader(FileName, EntryPoint, "ps_5_0");

    ID3D11PixelShader *psPtr;

    HR(DeviceKeeper::GetDevice()->CreatePixelShader(
        shaderBuffer->GetBufferPointer(),
        shaderBuffer->GetBufferSize(),
        NULL,
        &psPtr));

    ps = psPtr;
}

void PixelShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->PSSetShaderResources(Register, 1, &SRW);
}

void PixelShader::SetShader(BOOL CleanUp) const
{
    if(ps.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->PSSetShader((CleanUp) ? NULL : ps, NULL, 0);
}

void PixelShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->PSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void PixelShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->PSSetSamplers(Register, 1, &SamplerState);
}

void PixelShader::FreeData()
{
    Shader::FreeData();
    ps.Release();
}

void PixelShader::Construct(const PixelShader &Val)
{
    Shader::Construct(Val);
    ps = Val.ps;   
}

PixelShader::PixelShader(const PixelShader &Val)
{    
    Construct(Val);
}

PixelShader &PixelShader::operator= (const PixelShader &Val)
{    
    Construct(Val);

    return *this;
}

void PixelShader::ConstructAsRef(const PixelShader &Val)
{
    Shader::ConstructAsRef(Val);
    ps = Val.ps;
}

void VertexShader::Load(const std::wstring &FileName, const std::string &EntryPoint, const Meshes::VertexMetadata &InputLayout) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> shaderBuffer = CompileShader(FileName, EntryPoint, "vs_5_0");

    ID3D11VertexShader *vsPtr;

    HR(DeviceKeeper::GetDevice()->CreateVertexShader(
        shaderBuffer->GetBufferPointer(),
        shaderBuffer->GetBufferSize(),
        NULL, 
        &vsPtr));

    vs = vsPtr;

    ID3D11InputLayout *layoutPtr;

    HR(DeviceKeeper::GetDevice()->CreateInputLayout(
        &*InputLayout.begin(),
        InputLayout.Size(),
        shaderBuffer->GetBufferPointer(),
        shaderBuffer->GetBufferSize(),
        &layoutPtr));

    layout = layoutPtr;
}

void VertexShader::FreeData()
{
    Shader::FreeData();

    vs.Release();
    layout.Release();
}

void VertexShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->VSSetShaderResources(Register, 1, &SRW);
}

void VertexShader::SetShader(BOOL CleanUp) const
{
    if(vs.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->VSSetShader((CleanUp) ? NULL : vs, NULL, 0);
}

void VertexShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->VSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void VertexShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->VSSetSamplers(Register, 1, &SamplerState);
}

void VertexShader::Apply()
{
    Shader::Apply();
    DeviceKeeper::GetDeviceContext()->IASetInputLayout(layout);
}

void VertexShader::Construct(const VertexShader &Val)
{
    Shader::Construct(Val);

    vs = Val.vs;
    layout = Val.layout;
}

void VertexShader::ConstructAsRef(const VertexShader &Val)
{
    Shader::ConstructAsRef(Val);

    vs = Val.vs;
    layout = Val.layout;
}

VertexShader::VertexShader(const VertexShader &Val)
{    
    Construct(Val);
}

VertexShader &VertexShader::operator= (const VertexShader &Val)
{    
    Construct(Val);
    return *this;
}

bool Shader::VariableDescriptionKey::operator< (const VariableDescriptionKey &V)  const
{
    if(regId < V.regId)
        return true;
    else if(regId > V.regId)
        return false;
        
    return pos < V.pos;
}

void Shader::CreateConstantBuffer(RegisterType Register, UINT BufferSize) throw (Exception)
{
    Utils::SharedCOM<ID3D11Buffer> buffer = Utils::DirectX::CreateBuffer(BufferSize, 
                                                                         D3D11_BIND_CONSTANT_BUFFER, 
                                                                         D3D11_USAGE_DYNAMIC, 
                                                                         D3D11_CPU_ACCESS_WRITE);

    constantBuffers[Register] = buffer;
}

void Shader::CreateSamplerState(const std::string &Name, RegisterType Register)
{
    SamplerStateData smpStateData;
    smpStateData.regId = Register;

    samplerStates[Name] = smpStateData;
}


void Shader::CreateSamplerState(const std::string &Name,
                                RegisterType Register,
                                Utils::SharedCOM<ID3D11SamplerState> State)
{
    SamplerStateData smpStateData;
    smpStateData.regId = Register;
    smpStateData.state = State;

    samplerStates[Name] = smpStateData;
}

void Shader::CreateShaderResourceViewVariable(RegisterType Register,
                                              Utils::SharedCOM<ID3D11SamplerState> State) throw (Exception)
{
    std::string samplerStateName = "Register" + Utils::ToString(Register) + "SamplerState";

    CreateSamplerState(samplerStateName, Register, State);

    varsSamplerStates[Register] = samplerStates[samplerStateName];
}

void Shader::CreateShaderResourceViewVariable(const std::string &VarName, RegisterType Register)
{
    srvVarsToRegsMapping[VarName] = Register;
}

void Shader::CreateShaderResourceViewVariable(const std::string &VarName,
                                              RegisterType Register,
                                              Utils::SharedCOM<ID3D11SamplerState> State) throw (Exception)
{
    std::string samplerStateName = VarName + "DefaultSamplerState";

    CreateShaderResourceViewVariable(VarName, Register);
    CreateSamplerState(samplerStateName, Register, State);

    varsSamplerStates[Register] = samplerStates[samplerStateName];
}

void Shader::CreateShaderResourceViewVariable(const std::string &VarName,
                                              RegisterType Register,
                                              const std::string &StateName) throw (Exception)
{
    SamplerStateData samplerState = Utils::Find(samplerStates, StateName, ShaderSamplerStateException("sampler state " + StateName + " not found"));

    CreateShaderResourceViewVariable(VarName, Register);

    varsSamplerStates[Register] = samplerState;
}

void Shader::BoundSamplerState(const std::string &VarName, const std::string &StateName)
{
    SamplerStateData samplerState = Utils::Find(samplerStates, StateName, ShaderSamplerStateException("sampler state " + StateName + " not found"));

    RegisterType reg = Utils::Find(srvVarsToRegsMapping, VarName, ShaderVariableException("variable " + VarName + " not found"));

    varsSamplerStates[reg] = samplerState;
}

void Shader::FreeData()
{
    resourcesToClear.clear();
    samplerStatesToClear.clear();

    constantBuffers.clear();
    samplerStates.clear();

    vars.clear();
    srvVarsToRegsMapping.clear();
    varsSamplerStates.clear();
}

void Shader::CreateRawVariable(const std::string &VarName, RegisterType RegId, INT Pos, INT SizeInBytes) throw (Exception)
{
    std::hash<std::string> hash;
    VariableId varId = hash(VarName);

    for(const auto &pair : vars){
        if(pair.first.varId == varId)
            throw ShaderVariableException("variable " + VarName + " already used");

        if(pair.first.pos == Pos && pair.first.regId == RegId)
            throw ShaderVariableException("parameters for variable " + VarName + " already used");
    }

    VariableDescriptionKey desc;
    desc.pos = Pos;
    desc.regId = RegId;
    desc.varId = varId;

    vars.insert({desc, std::vector<char>(SizeInBytes)});

}

void Shader::ApplyVariables() throw (Exception)
{
    if(!needToApply)
        return;

    if(vars.size() == 0)
        return;

    for(const auto &pair : vars)
        if(pair.second.size() == 0)
            throw ShaderVariableException("Not all variables initialized");

    std::vector<char> bufferData;

    INT prevRegId = vars.begin()->first.regId;

    for(const auto &pair : vars){
        
        INT regId = pair.first.regId;

        if(regId != prevRegId){

            SetBufferData(prevRegId, &bufferData[0], bufferData.size());

            bufferData.clear();

            prevRegId = regId;
        }

        bufferData.insert(bufferData.end(), pair.second.begin(), pair.second.end());
    }

    RegisterType lastRegId = vars.rbegin()->first.regId;
    SetBufferData(lastRegId, &bufferData[0], bufferData.size());

    needToApply = false;
}

void Shader::Apply()
{
    SetShader();

    if(needToApply)
        ApplyVariables();

    for(const auto &pair : GetConstantBuffers())
        SetConstantBuffer(pair.first, pair.second);


    for(const auto &pair : varsSamplerStates)
        SetSamplerState(pair.second.regId, pair.second.state);
}

void Shader::CleanUp()
{
    SetShader(true);
    ResetResources();
    ResetSamplerStates();
}

void Shader::Construct(const Shader &Val)
{        
    FreeData();

    vars = Val.vars;
    varsSamplerStates = Val.varsSamplerStates;
    srvVarsToRegsMapping = Val.srvVarsToRegsMapping;

    for(const auto &p : Val.constantBuffers)
        constantBuffers.insert({p.first, Utils::DirectX::CopyBuffer(p.second)});

    for(const auto &pair : Val.samplerStates){

        D3D11_SAMPLER_DESC desc;
        pair.second.state->GetDesc(&desc);

        ID3D11SamplerState *newState;
        HR(DeviceKeeper::GetDevice()->CreateSamplerState(&desc, &newState));

        SamplerStateData smpStateData;
        smpStateData.regId = pair.second.regId;
        smpStateData.state = newState;

        samplerStates.insert({pair.first, smpStateData});

        for(auto &var : varsSamplerStates)
            if(var.second.state == pair.second.state)
                var.second.state = smpStateData.state;
    }
}

void Shader::ConstructAsRef(const Shader &Val)
{
    FreeData();

    vars = Val.vars;

    constantBuffers = Val.constantBuffers;
    samplerStates = Val.samplerStates;
    varsSamplerStates = Val.varsSamplerStates;
    srvVarsToRegsMapping = Val.srvVarsToRegsMapping;
}  

void Shader::SetBufferData(RegisterType RegId, const char *Data, INT DataSize)
{
    if(constantBuffers.find(RegId) == constantBuffers.end())
        CreateConstantBuffer(RegId, DataSize);

    Utils::DirectX::Map<char>(constantBuffers[RegId], [&](char *RawData)
    {
        std::copy(Data, Data + DataSize, RawData);
    });
}

void Shader::SetVariableData(const std::string &VarName, const char *Data) throw (Exception)
{
    std::hash<std::string> hash;
    VariableId varId = hash(VarName);

    auto it = std::find_if(vars.begin(), vars.end(), [&](const VariablesData::value_type &v) -> bool
    {
        return v.first.varId == varId;
    });

    if(it == vars.end())
        throw ShaderVariableException("variable " + VarName + " not found");

    std::copy(Data, Data + it->second.size(), it->second.begin());

    needToApply = true;
}

void Shader::UpdateVariable(const std::string &VarName, const std::vector<Matrix4x4> &VariableData) throw (Exception)
{
    if(VariableData.size() == 0)
        throw ShaderException("Empty array for variable " + VarName);

    std::vector<Matrix4x4> trnMatrices;

    for(const Matrix4x4 &m : VariableData)
        trnMatrices.push_back(Matrix4x4::Transpose(m));

    SetVariableData(VarName, reinterpret_cast<const char*>(&trnMatrices[0]));
}

void Shader::UpdateVariable(const std::string &VarName, const D3DXMATRIX &VariableData) throw (Exception)
{        
    D3DXMATRIX mTrans = Math::Transpose(VariableData);
    const char *ptr = reinterpret_cast<const char*>(&mTrans);

    SetVariableData(VarName, ptr);
}

void Shader::UpdateVariable(const std::string &VarName, const Matrix4x4 &VariableData) throw (Exception)
{
    Matrix4x4 mTrans = Matrix4x4::Transpose(VariableData);
    const char *ptr = reinterpret_cast<const char*>(&mTrans);

    SetVariableData(VarName, ptr);
}

void Shader::UseSamplerState(const std::string &VarName, ID3D11SamplerState *SamplerState) throw (Exception)
{
    RegisterType reg = Utils::Find(srvVarsToRegsMapping, VarName, ShaderVariableException("variable " + VarName + " not found"));

    const SamplerStateData &smpStateData = Utils::Find(varsSamplerStates, reg, ShaderVariableException("sampler state for variable " + VarName + " not found"));

    SetSamplerState(smpStateData.regId, SamplerState);

    if(std::find(samplerStatesToClear.begin(), samplerStatesToClear.end(), reg) == samplerStatesToClear.end())
        samplerStatesToClear.push_back(reg);
}

void Shader::UseSamplerState(const std::string &VarName, const std::string &StateName) throw (Exception)
{
    const SamplerStateData &smpStateData = Utils::Find(samplerStates, StateName, ShaderSamplerStateException("sampler state " + StateName + " not found"));

    UseSamplerState(VarName, smpStateData.state);
}

void Shader::UseResource(RegisterType Register, ID3D11ShaderResourceView *SRW)
{
    SetShaderResourceView(Register, SRW);

    if(std::find(resourcesToClear.begin(), resourcesToClear.end(), Register) == resourcesToClear.end())
        resourcesToClear.push_back(Register);
}

void Shader::UseResource(const std::string &VarName, ID3D11ShaderResourceView *SRW) throw (Exception)
{
    RegisterType reg = Utils::Find(srvVarsToRegsMapping, VarName, ShaderVariableException("variable " + VarName + " not found"));

    UseResource(reg, SRW);
}

void Shader::ResetResources()
{
    for(RegisterType reg : resourcesToClear)
        SetShaderResourceView(reg, NULL);

    resourcesToClear.clear();
}

void Shader::ResetSamplerStates()
{
    for(RegisterType reg : samplerStatesToClear)
        SetSamplerState(reg, varsSamplerStates[reg].state);

    samplerStatesToClear.clear();
}

void DomainShader::Construct(const DomainShader &Val)
{
    Shader::Construct(Val);
    
    ds = Val.ds;
}

void DomainShader::ConstructAsRef(const DomainShader &Val)
{
    Shader::ConstructAsRef(Val);

    ds = Val.ds;
};

DomainShader::DomainShader(const DomainShader &Val)
{
    Construct(Val);
}

DomainShader &DomainShader::operator= (const DomainShader &Val)
{
    Construct(Val);
    return *this;
}

void DomainShader::FreeData()
{
    Shader::FreeData();
    ds.Release();
}

void DomainShader::Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> buffer = CompileShader(FileName, EntryPoint, "ds_5_0");

    ID3D11DomainShader *dsPtr;

    HR(DeviceKeeper::GetDevice()->CreateDomainShader(
        buffer->GetBufferPointer(),
        buffer->GetBufferSize(),
        NULL, 
        &dsPtr));

    ds = dsPtr;
}

void DomainShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->DSSetShaderResources(Register, 1, &SRW);
}

void DomainShader::SetShader(BOOL CleanUp) const
{
    if(ds.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->DSSetShader((CleanUp) ? NULL : ds, NULL, 0);
}

void DomainShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->DSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void DomainShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->DSSetSamplers(Register, 1, &SamplerState);
}

void HullShader::Construct(const HullShader &Val)
{
    Shader::Construct(Val);
    
    hs = Val.hs;
}

void HullShader::ConstructAsRef(const HullShader &Val)
{
    Shader::ConstructAsRef(Val);

    hs = Val.hs;
}

HullShader::HullShader(const HullShader &Val)
{
    Construct(Val);
}

HullShader &HullShader::operator= (const HullShader &Val)
{
    Construct(Val);
    return *this;
}

void HullShader::FreeData()
{
    Shader::FreeData();

    hs.Release();
}

void HullShader::Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> buffer = CompileShader(FileName, EntryPoint, "hs_5_0");

    ID3D11HullShader *hsPtr;

    HR(DeviceKeeper::GetDevice()->CreateHullShader(
        buffer->GetBufferPointer(),
        buffer->GetBufferSize(),
        NULL,
        &hsPtr));

    hs = hsPtr;
}

void HullShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->HSSetShaderResources(Register, 1, &SRW);
}

void HullShader::SetShader(BOOL CleanUp) const
{
    if(hs.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->HSSetShader((CleanUp) ? NULL : hs, NULL, 0);
}

void HullShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->HSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void HullShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->HSSetSamplers(Register, 1, &SamplerState);
}

void GeometryShader::Construct(const GeometryShader &Val)
{
    Shader::Construct(Val);
    
    gs = Val.gs;
}

void GeometryShader::ConstructAsRef(const GeometryShader &Val)
{
    Shader::ConstructAsRef(Val);
    
    gs = Val.gs;
}

GeometryShader::GeometryShader(const GeometryShader &Val)
{
    Construct(Val);
}

GeometryShader &GeometryShader::operator= (const GeometryShader &Val)
{
    Construct(Val);
    return *this;
}

void GeometryShader::FreeData()
{
    Shader::FreeData();
    gs.Release();
}

void GeometryShader::Load(const std::wstring &FileName, const std::string &EntryPoint, const StreamOutMetadata &InputSOMetadata) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> buffer = CompileShader(FileName, EntryPoint, "gs_5_0");
    
    ID3D11GeometryShader *gsPtr;

    HR(DeviceKeeper::GetDevice()->CreateGeometryShaderWithStreamOutput(
        buffer->GetBufferPointer(),
        buffer->GetBufferSize(),
        &InputSOMetadata[0],
        InputSOMetadata.size(),
        NULL, 
        0,
        0,
        NULL,
        &gsPtr));

    gs = gsPtr;
}

void GeometryShader::Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> buffer = CompileShader(FileName, EntryPoint, "gs_5_0");

    ID3D11GeometryShader *gsPtr;

    HR(DeviceKeeper::GetDevice()->CreateGeometryShader(
        buffer->GetBufferPointer(),
        buffer->GetBufferSize(),
        NULL,
        &gsPtr));

    gs = gsPtr;
}

void GeometryShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->GSSetShaderResources(Register, 1, &SRW);
}

void GeometryShader::SetShader(BOOL CleanUp) const
{
    if(gs.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->GSSetShader((CleanUp) ? NULL : gs, NULL, 0);
}

void GeometryShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->GSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void GeometryShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->GSSetSamplers(Register, 1, &SamplerState);
}

void ComputeShader::Construct(const ComputeShader &Val)
{
    Shader::Construct(Val);

    uawsVarsToRegsMapping = Val.uawsVarsToRegsMapping;
    cs = Val.cs;
}

void ComputeShader::ConstructAsRef(const ComputeShader &Val)
{
    Shader::ConstructAsRef(Val);
    
    uawsVarsToRegsMapping = Val.uawsVarsToRegsMapping;
    cs = Val.cs;
}

void ComputeShader::FreeData()
{
    Shader::FreeData();

    uawsToClear.clear();
    uawsVarsToRegsMapping.clear();

    cs.Release();
}

void ComputeShader::SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const
{
    DeviceKeeper::GetDeviceContext()->CSSetShaderResources(Register, 1, &SRW);
}

void ComputeShader::SetShader(BOOL CleanUp) const
{
    if(cs.Get() == nullptr)
        return;

     DeviceKeeper::GetDeviceContext()->CSSetShader((CleanUp) ? NULL : cs, NULL, 0);
}

void ComputeShader::SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const
{
    DeviceKeeper::GetDeviceContext()->CSSetConstantBuffers(Register, 1, &ConstantBuffer);
}

void ComputeShader::SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const
{
    DeviceKeeper::GetDeviceContext()->CSSetSamplers(Register, 1, &SamplerState);
}

ComputeShader::ComputeShader(const ComputeShader &Val)
{
    Construct(Val);
}

ComputeShader &ComputeShader::operator= (const ComputeShader &Val)
{
    Construct(Val);
    return *this;
}

void ComputeShader::Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception)
{
    FreeData();

    Utils::AutoCOM<ID3D10Blob> buffer = CompileShader(FileName, EntryPoint, "cs_5_0");

    ID3D11ComputeShader *csPtr;

    HR(DeviceKeeper::GetDevice()->CreateComputeShader(
        buffer->GetBufferPointer(),
        buffer->GetBufferSize(),
        NULL,
        &csPtr));

    cs = csPtr;
}

void ComputeShader::CreateUnorderedAccessViewVariable(const std::string &VarName, RegisterType Register) throw (Exception)
{
    uawsVarsToRegsMapping[VarName] = Register;
}

void ComputeShader::UseResource(const std::string &VarName, ID3D11UnorderedAccessView *UAW, UINT InitialCount) throw (Exception)
{
    RegisterType reg = Utils::Find(uawsVarsToRegsMapping, VarName, [&](){return ShaderVariableException("variable " + VarName + " not found");});

    DeviceKeeper::GetDeviceContext()->CSSetUnorderedAccessViews(reg, 1, &UAW, &InitialCount);

    if(std::find(uawsToClear.begin(), uawsToClear.end(), reg) == uawsToClear.end())
        uawsToClear.push_back(reg);
}

void ComputeShader::ResetResources()
{
    Shader::ResetResources();

    ID3D11UnorderedAccessView *nullUAW = nullptr;

    for(RegisterType reg : uawsToClear)
        DeviceKeeper::GetDeviceContext()->CSSetUnorderedAccessViews(reg, 1, &nullUAW, nullptr );

    uawsToClear.clear();
}

}
