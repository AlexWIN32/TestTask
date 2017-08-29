/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once 
#include <Exception.h>
#include <D3DHeaders.h>
#include <DeviceKeeper.h>
#include <MeshesFwd.h>
#include <Utils/SharedCOM.h>
#include <map>
#include <vector>
#include <sstream>

class Matrix4x4;

namespace Shaders
{

DECLARE_EXCEPTION(ShaderException)
DECLARE_CHILD_EXCEPTION(ShaderCompilationException, ShaderException)
DECLARE_CHILD_EXCEPTION(ShaderVariableException, ShaderException)
DECLARE_CHILD_EXCEPTION(ShaderSamplerStateException, ShaderException)

typedef INT RegisterType;

typedef std::vector<D3D11_SO_DECLARATION_ENTRY> StreamOutMetadata;

class Shader
{
private:	
    typedef INT VariableId;
    struct VariableDescriptionKey
    {
        RegisterType regId = 0;
        INT pos = 0;
        VariableId varId = 0;
        bool operator< (const VariableDescriptionKey &V) const;
    };
    struct SamplerStateData
    {
        RegisterType regId = 0;
        Utils::SharedCOM<ID3D11SamplerState> state;
    };
    typedef std::map<RegisterType, Utils::SharedCOM<ID3D11Buffer>> ConstantBuffersStorage;
    typedef std::map<std::string, SamplerStateData> SamplerStatesStorage;
    typedef std::map<VariableDescriptionKey, std::vector<char>> VariablesData;
    typedef std::vector<RegisterType> ResourcesToClearStorage;
    typedef std::vector<RegisterType> SamplerStatesToClearStorage;
    typedef std::map<std::string, RegisterType> SRVVarsToRegsMappingStorage;
    typedef std::map<RegisterType, SamplerStateData> VarsSamplerStatesMapping;
    VariablesData vars;
    ConstantBuffersStorage constantBuffers;
    SamplerStatesStorage samplerStates;
    ResourcesToClearStorage resourcesToClear;
    SamplerStatesToClearStorage samplerStatesToClear;
    SRVVarsToRegsMappingStorage srvVarsToRegsMapping;
    VarsSamplerStatesMapping varsSamplerStates;
    bool needToApply = false;
    void SetBufferData(RegisterType RegId, const char *Data, INT DataSize);
    void SetVariableData(const std::string &VarName, const char *Data) throw (Exception);
protected:
	const SamplerStatesStorage &GetSamplerStates() const { return samplerStates; }
	const ConstantBuffersStorage &GetConstantBuffers() const { return constantBuffers; }    
    void Construct(const Shader &Val);
    void ConstructAsRef(const Shader &Val);
    virtual void FreeData() = 0;
	virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const = 0;
    virtual void SetShader(BOOL CleanUp = false) const = 0;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const = 0;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const = 0;
public:
    virtual ~Shader(){}
    void CreateRawVariable(const std::string &VarName, RegisterType RegId, INT Pos, INT SizeInBytes) throw (Exception);
    template<class TVar>
    void CreateVariable(const std::string &VarName, RegisterType RegId, INT Pos, const TVar &DefaultValue = TVar()) throw (Exception)
    {
        CreateRawVariable(VarName, RegId, Pos, sizeof(TVar));
        UpdateVariable(VarName, DefaultValue);
    }
    template<class TVar>
    void CreateVariable(const std::string &VarName, RegisterType RegId, INT Pos, const std::vector<TVar> &Value) throw (Exception)
    {
        if(Value.size() == 0)
            throw ShaderException("Empty array for variable " + VarName);

        CreateRawVariable(VarName, RegId, Pos, sizeof(TVar) * Value.size());
        UpdateVariable(VarName, Value);
    }
	void CreateConstantBuffer(RegisterType Register, UINT BufferSize) throw (Exception); //DEPRECATED
    void CreateSamplerState(const std::string &Name, RegisterType Register);
    void CreateSamplerState(const std::string &Name,
                            RegisterType Register,
                            Utils::SharedCOM<ID3D11SamplerState> State);
    void CreateShaderResourceViewVariable(RegisterType Register,
                                          Utils::SharedCOM<ID3D11SamplerState> State) throw (Exception);//DEPRECATED
    void CreateShaderResourceViewVariable(const std::string &VarName,
                                          RegisterType Register,
                                          Utils::SharedCOM<ID3D11SamplerState> State) throw (Exception);
    void CreateShaderResourceViewVariable(const std::string &VarName,
                                          RegisterType Register,
                                          const std::string &StateName) throw (Exception);
    void CreateShaderResourceViewVariable(const std::string &VarName, RegisterType Register);
    void BoundSamplerState(const std::string &VarName, const std::string &StateName);
    void ApplyVariables() throw (Exception);
	template<class TBuffer>
	void UpdateConstantBuffer(RegisterType Register, const TBuffer &BufferData) throw (Exception) //DEPRECATED
	{
        const char* ptr = reinterpret_cast<const char*>(&BufferData);
        SetBufferData(Register, ptr, sizeof(TBuffer));
	}
    template<class TVar>
    void UpdateVariable(const std::string &VarName, const TVar &VariableData) throw (Exception)
    {
        SetVariableData(VarName, reinterpret_cast<const char*>(&VariableData));
    }    
    template<class TVar>
    void UpdateVariable(const std::string &VarName, const std::vector<TVar> &VariableData) throw (Exception)
    {
        if(VariableData.size() == 0)
            throw ShaderException("Empty array for variable " + VarName);

        SetVariableData(VarName, reinterpret_cast<const char*>(&VariableData[0]));
    }
    void UpdateVariable(const std::string &VarName, const std::vector<Matrix4x4> &VariableData) throw (Exception);
    void UpdateVariable(const std::string &VarName, const D3DXMATRIX &VariableData) throw (Exception);
    void UpdateVariable(const std::string &VarName, const Matrix4x4 &VariableData) throw (Exception);
	virtual void Apply();
    void UseResource(RegisterType Register, ID3D11ShaderResourceView *SRW); //DEPRECATED
    void UseResource(const std::string &VarName, ID3D11ShaderResourceView *SRW) throw (Exception);
    void UseSamplerState(const std::string &VarName, ID3D11SamplerState *SamplerState) throw (Exception);
    void UseSamplerState(const std::string &VarName, const std::string &StateName) throw (Exception);
    virtual void ResetResources();
    virtual void ResetSamplerStates();
    void CleanUp();
};

class PixelShader final : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11PixelShader> ps;
protected:
    void Construct(const PixelShader &Val);
    virtual void FreeData();
	virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    void ConstructAsRef(const PixelShader &Val);
    PixelShader(const PixelShader &Val);
    PixelShader &operator= (const PixelShader &Val);
    virtual ~PixelShader(){}
	PixelShader(){}
	void Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception);
};

class VertexShader final : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11VertexShader> vs;
    Utils::SharedCOM<ID3D11InputLayout> layout;
protected:
    void Construct(const VertexShader &Val);
    virtual void FreeData();
	virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    void ConstructAsRef(const VertexShader &Val);
    VertexShader(const VertexShader &Val);
    VertexShader &operator= (const VertexShader &Val);
    virtual ~VertexShader(){}
	VertexShader(){}
	void Load(const std::wstring &FileName, const std::string &EntryPoint, const Meshes::VertexMetadata &InputLayout) throw (Exception);
    virtual void Apply();
};

class DomainShader final : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11DomainShader> ds;
protected:
    void Construct(const DomainShader &Val);
    virtual void FreeData();
    virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    void ConstructAsRef(const DomainShader &Val);
    DomainShader(const DomainShader &Val);
    DomainShader &operator= (const DomainShader &Val);
    virtual ~DomainShader(){}
    DomainShader(){}
    void Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception);
};

class HullShader final : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11HullShader> hs;
protected:
    void Construct(const HullShader &Val);
    virtual void FreeData();
    virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    void ConstructAsRef(const HullShader &Val);
    HullShader(const HullShader &Val);
    HullShader &operator= (const HullShader &Val);
    virtual ~HullShader(){}
    HullShader(){}
    void Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception);
};

class GeometryShader : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11GeometryShader> gs;
protected:
    void Construct(const GeometryShader &Val);
    virtual void FreeData();
    virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    void ConstructAsRef(const GeometryShader &Val);
    GeometryShader(const GeometryShader &Val);
    GeometryShader &operator= (const GeometryShader &Val);
    virtual ~GeometryShader(){}
    GeometryShader(){}
    void Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception);
    void Load(const std::wstring &FileName, const std::string &EntryPoint, const StreamOutMetadata &InputSOMetadata) throw (Exception);
};

class ComputeShader final : public Shader
{
private:
    mutable Utils::SharedCOM<ID3D11ComputeShader> cs;
    std::vector<RegisterType> uawsToClear;
    std::map<std::string, RegisterType> uawsVarsToRegsMapping;
protected:
    void Construct(const ComputeShader &Val);
    virtual void FreeData();
    virtual void SetShaderResourceView(RegisterType Register, ID3D11ShaderResourceView *SRW) const;
    virtual void SetShader(BOOL CleanUp = false) const;
    virtual void SetConstantBuffer(RegisterType Register, ID3D11Buffer *ConstantBuffer) const;
    virtual void SetSamplerState(RegisterType Register, ID3D11SamplerState *SamplerState) const;
public:
    using Shader::UseResource;
    void ConstructAsRef(const ComputeShader &Val);
    ComputeShader(const ComputeShader &Val);
    ComputeShader &operator= (const ComputeShader &Val);
    virtual ~ComputeShader(){}
    ComputeShader(){}
    void Load(const std::wstring &FileName, const std::string &EntryPoint) throw (Exception);
    void UseResource(const std::string &VarName, ID3D11UnorderedAccessView *UAW, UINT InitialCount) throw (Exception);
    void CreateUnorderedAccessViewVariable(const std::string &VarName, RegisterType Register) throw (Exception);
    virtual void ResetResources();
};

struct ShadersSet
{
    VertexShader vs;
    PixelShader ps;
    GeometryShader gs;
    HullShader hs;
    DomainShader ds;
};

template<class TFunc>
inline void Apply(Shaders::ShadersSet &Shaders, TFunc Function)
{
    Shaders.vs.Apply();
    Shaders.hs.Apply();
    Shaders.ds.Apply();
    Shaders.gs.Apply();
    Shaders.ps.Apply();

    Function();

    Shaders.vs.CleanUp();
    Shaders.hs.CleanUp();
    Shaders.ds.CleanUp();
    Shaders.gs.CleanUp();
    Shaders.ps.CleanUp();
}

inline void DispatchComputeShader(ComputeShader &Shader,
                                  UINT ThreadGroupCountX,
                                  UINT ThreadGroupCountY,
                                  UINT ThreadGroupCountZ)
{
    Shader.Apply();

    DeviceKeeper::GetDeviceContext()->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);

    Shader.CleanUp();
}

};
