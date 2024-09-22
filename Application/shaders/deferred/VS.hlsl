#include "../Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    
};


struct VS_OUTPUT
{
	
    float4 positionCS : SV_POSITION;
    float3 positionWS : POSITION0;
    float3 normal : NORMAL;
	
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
   
    float4 worldPos = mul(gObjectData.transform, float4(input.position, 1.0f));
    output.positionWS = worldPos.xyz;
    output.normal = mul((float3x3) gObjectData.transform, input.normal);
      
    output.positionCS = mul(gPassData.viewproj, worldPos);
     
    return output;
}
