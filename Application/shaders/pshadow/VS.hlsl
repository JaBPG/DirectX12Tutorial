#include "../Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    
};


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
  
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    
    float4 worldProjectPos = mul(gObjectData.transform, float4(input.position, 1.0f));

    output.position = mul(gPassData.viewproj, worldProjectPos);
    
    
    return output;
}
