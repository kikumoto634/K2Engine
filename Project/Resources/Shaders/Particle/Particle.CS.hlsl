//[numthreads(1, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{
//}


// ルートシグネチャの宣言
#define RS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
                    "DescriptorTable(UAV(u0, numDescriptors = 1, space = 0, offset = DESCRIPTOR_RANGE_OFFSET_APPEND), "\
                                    "visibility = SHADER_VISIBILITY_ALL),"\
                    "StaticSampler(s0, "\
                                  "filter         = FILTER_MIN_MAG_MIP_LINEAR, "\
                                  "addressU       = TEXTURE_ADDRESS_WRAP, "\
                                  "addressV       = TEXTURE_ADDRESS_WRAP, "\
                                  "addressW       = TEXTURE_ADDRESS_WRAP, "\
                                  "mipLodBias     = 0.0f, "\
                                  "maxAnisotropy  = 0, "\
                                  "comparisonFunc = COMPARISON_NEVER, "\
                                  "borderColor    = STATIC_BORDER_COLOR_TRANSPARENT_BLACK, "\
                                  "minLOD         = 0.0f, "\
                                  "maxLOD         = 3.402823466e+38f, "\
                                  "space          = 0, "\
                                  "visibility     = SHADER_VISIBILITY_ALL)"

struct Sample
{
    float3 poaition;
    float3 velocity;
};
RWStructuredBuffer<Sample> real : register(u0);

[RootSignature(RS)]
[numthreads(1, 1, 1)]
void main(uint3 gID : SV_GroupID)
{
    if (real[gID.x].poaition.r > 3.0f || real[gID.x].poaition.r < -3.0f)
    {
        real[gID.x].velocity.r *= -1.0f;
    }
    if (real[gID.x].poaition.g > 8.0f || real[gID.x].poaition.g < 2.0f)
    {
        real[gID.x].velocity.g *= -1.0f;
    }
    if (real[gID.x].poaition.b > 3.0f || real[gID.x].poaition.b < -3.0f)
    {
        real[gID.x].velocity.b *= -1.0f;
    }
    
    real[gID.x].poaition.r += real[gID.x].velocity.r;
    real[gID.x].poaition.g += real[gID.x].velocity.g;
    real[gID.x].poaition.b += real[gID.x].velocity.b;
}