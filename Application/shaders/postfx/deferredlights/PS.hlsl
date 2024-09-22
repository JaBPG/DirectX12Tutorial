struct VS_OUTPUT
{
    float4 posCS : SV_Position;
    float2 uvs : TEXCOORD;
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

Texture2D gPositionBuffer : register(t0);
Texture2D gNormalBuffer : register(t1);
Texture2D gAlbedoBuffer : register(t2);


SamplerState sPointClamp : register(s0);
SamplerState sLinearClamp : register(s1);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 normal = gNormalBuffer.Sample(sPointClamp, input.uvs).xyz;
    float4 albedo = gAlbedoBuffer.Sample(sPointClamp, input.uvs);
    
    float intensity = dot(-(normalize(gPassData.light.direction)), normalize(normal));
   
    float4 color = 1.0f; 
    
    
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
   
    return color * albedo;
}



/*

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

*/