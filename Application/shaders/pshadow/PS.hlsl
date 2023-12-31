//our shadow pixelshader
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
    float dp = dot((normalize(gPassData.light.direction)), normalize(input.normal));
    
    if (dp > 0)
    {
        return gMaterialData.diffuseAlbedo;
    }
    else
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    
   
}