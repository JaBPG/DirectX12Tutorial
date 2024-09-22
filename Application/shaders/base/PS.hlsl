#include "../Common.hlsli"

struct PS_INPUT //from the VS_OUTPUT
{
	
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
	
};

struct MaterialData
{
    float4 diffuseAlbedo;
    
};

ConstantBuffer<MaterialData> gMaterialData : register(b2);

float4 main(PS_INPUT input) : SV_TARGET
{
    float intensity;
    float4 color;
    
    intensity = dot(-(normalize(gPassData.light.direction)), normalize(input.normal));
    
    if (intensity > 0.90)
    {
        color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (intensity > 0.5)
    {
        color = float4(.6f, 0.6f, 0.6f, 1.0f);
        
    }
    else if (intensity > 0.25)
    {
        color = float4(.4f, 0.4f, 0.4f, 1.0f);
        
    }
    else 
    {
        color = float4(.05f, 0.05f, 0.05f, 1.0f);
    }
   
    return color  * gMaterialData.diffuseAlbedo;
   
}