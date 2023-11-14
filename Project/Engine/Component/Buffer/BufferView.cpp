#include "BufferView.h"

void CreateBufferView(D3D12_VERTEX_BUFFER_VIEW &view, ID3D12Resource *resource, UINT sizeInByte, UINT strideInBytes)
{
	//リソースの先頭アドレスから使用
	view.BufferLocation = resource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点引数分のサイズ
	view.SizeInBytes = sizeInByte;
	//1頂点当たりのサイズ
	view.StrideInBytes = strideInBytes;
}

void CreateBufferView(D3D12_INDEX_BUFFER_VIEW &view, ID3D12Resource *resource, UINT sizeInByte)
{
	//リソースの先頭アドレスから使う
	view.BufferLocation = resource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックスは引数分
	view.SizeInBytes = sizeInByte;
	//インデックスはuint32_tとする
	view.Format = DXGI_FORMAT_R32_UINT;
}
