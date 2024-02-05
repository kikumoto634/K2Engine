#include "ObjModel.h"
#include <imgui.h>
#include "SpriteLoader.h"

ObjModel *ObjModel::Create(std::string filePath, Transform transform, BlendSetting::BlendMode mode)
{
	ObjModel *instance = new ObjModel(filePath, transform,mode);
	instance->ObjModelLoad();
	instance->Initialize(false);
	instance->ObjModelVertexData();
	return instance;
}

void ObjModel::ObjModelLoad()
{
	//モデル読み込み
	modelData_ = LoadObjFile(filePath_);
	//頂点数決め
	vertNum_ = (UINT)modelData_.vertices.size();
	//画像パス
	texturePath_ = modelData_.material.textureFilePath;
	SpriteLoader::LoadTexture(DirectXCommon::GetInstance(), texturePath_);
}

void ObjModel::ObjModelVertexData()
{
	//データ送信
	std::memcpy(vertData_, modelData_.vertices.data(), sizeof(VertexData) * vertNum_);
}

bool ObjModel::PipelineCreate()
{
#pragma region サンプル
	/*pipelineDatas.pipeline_ = std::make_unique<Pipeline>();

	//SRV(Texture)
	D3D12_DESCRIPTOR_RANGE SRVDescriptorRange[1] = {};
	SRVDescriptorRange[0].BaseShaderRegister = 0;	//0から開始
	SRVDescriptorRange[0].NumDescriptors = 1;	//数は1
	SRVDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータ設定
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.resize(4);

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = SRVDescriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(SRVDescriptorRange);

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[2].Descriptor.ShaderRegister = 1;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;


	//Sampler設定(シェーダーのPS SamplerState　シェーダでは画像のことをいう)
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	staticSamplers.resize(1);
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	
	//インプットレイアウト設定(頂点データでシェーダ内に送るデータたちのセマンティック名)
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc;
	inputElementDesc.resize(3);
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";	
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	pipelineDatas.rootParameters_ = rootParameters;
	pipelineDatas.staticSamplers_ = staticSamplers;
	pipelineDatas.inputElementDesc_ = inputElementDesc;

	pipelineDatas.VSPath_ = "Object3D/Object3D.VS.hlsl";
	pipelineDatas.PSPath_ = "Object3D/Object3D.PS.Texture.hlsl";

	//生成
	std::wstring vs = WindowsApp::ConvertString(pipelineDatas.VSPath_);
	std::wstring ps = WindowsApp::ConvertString(pipelineDatas.PSPath_);

	pipelineDatas.pipeline_->DepthStencilSet();
	pipelineDatas.pipeline_->Create(
		vs,
		ps,
		rootParameters,
		staticSamplers,
		inputElementDesc,
		pipelineDatas.fillMode_,
		pipelineDatas.pipelinePrimitiveTopology_,
		pipelineDatas.blendMode,
		pipelineDatas.cullingMode
	);

	return true;*/
#pragma endregion
	return false;
}
