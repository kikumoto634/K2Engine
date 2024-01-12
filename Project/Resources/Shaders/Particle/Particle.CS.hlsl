//[numthreads(1, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{
//}

RWStructuredBuffer<float> real : register(u0);

[RootSignature(RS)]
[numthreads(1, 1, 1)]
void main(uint3 gID : SV_GroupID)
{
    //共有データに配列番号が入る
    real[gID.x] = gID.x;
}