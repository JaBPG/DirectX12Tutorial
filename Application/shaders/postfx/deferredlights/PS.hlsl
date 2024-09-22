#include "../../Common.hlsli"

struct VS_OUTPUT
{
    float4 posCS : SV_Position;
    float2 uvs : TEXCOORD;
};


Texture2D gPositionBuffer : register(t0);
Texture2D gNormalBuffer : register(t1);
Texture2D gAlbedoBuffer : register(t2);
Texture2D gGDepthBuffer : register(t3);

Texture2D gShadowMapBuffer : register(t4);

SamplerState sPointClamp : register(s0,space0);
SamplerState sLinearClamp : register(s1,space0);
SamplerComparisonState sShadowCompare : register(s0, space1);

float CalculateShadowFactor(float4 lightclipspacepos)
{
    const float4x4 touvcoordinates =
    {
        float4(0.5f, 0.0f, 0.0f, 0.0f),
        float4(0.0f, -0.5f, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(0.5f, 0.5f, 0.0f, 1.0f)
    };
    
    float4 uvcooridnates = mul(lightclipspacepos, touvcoordinates);
    
    unsigned int width, height;
    gShadowMapBuffer.GetDimensions(width, height);
    
    float deltacoordinate = 1.0f / (float) width;
    
    const unsigned int kernelsize = 9;
    
    const float2 kernel[kernelsize] =
    {
        float2(-deltacoordinate, -deltacoordinate), float2(0.0f, -deltacoordinate), float2(deltacoordinate, -deltacoordinate),
        float2(-deltacoordinate, 0.0f), float2(0.0f, 0.0f), float2(deltacoordinate, 0.0f),
        float2(-deltacoordinate, deltacoordinate), float2(0.0f, deltacoordinate), float2(deltacoordinate, deltacoordinate)
        
    };
    
    float lightacc = 1.0f;
    
    [unroll]
    for (unsigned int i = 0; i < kernelsize; i++)
    {
        lightacc += gShadowMapBuffer.SampleCmpLevelZero(sShadowCompare, uvcooridnates.xy + kernel[i], uvcooridnates.z).r;
    }
    
    
    return lightacc / (float)kernelsize;
}


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 normal = gNormalBuffer.Sample(sPointClamp, input.uvs).xyz;
    float4 albedo = gAlbedoBuffer.Sample(sPointClamp, input.uvs);
    float3 position = gPositionBuffer.Sample(sPointClamp, input.uvs).xyz;
       
    
    float4 objposcslight = mul(gPassData.lightviewproj, float4(position, 1.0f));
    
    float shadowfactor = CalculateShadowFactor(objposcslight);
    
    float intensity = dot(-(normalize(gPassData.light.direction)), normalize(normal));
   
  
    float4 factor = 1.0f; 
    
    if (intensity > 0.90)
    {
        factor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (intensity > 0.5)
    {
        factor = float4(.6f, 0.6f, 0.6f, 1.0f);
        
    }
    else if (intensity > 0.25)
    {
        factor = float4(.4f, 0.4f, 0.4f, 1.0f);
    }
    else
    {
        factor = float4(.05f, 0.05f, 0.05f, 1.0f);
    }
    
    
    float4 color = (shadowfactor * albedo * 0.5 + intensity * albedo) / 1.5f;
    
    return color;
    
}