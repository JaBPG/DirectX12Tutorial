//Base pixel shader

struct PS_INPUT 
{
	
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
	
};



struct MaterialData
{
    float4 diffuseAlbedo;
    
};

struct LightData
{
    float3 position;
    float strength;
    float3 direction;
    float padding;

};

struct PassData
{
    float4x4 viewproj;
    LightData light;
};

ConstantBuffer<PassData> gPassData : register(b0);
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