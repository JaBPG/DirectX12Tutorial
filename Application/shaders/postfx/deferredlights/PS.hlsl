#include "../../Common.hlsli"

struct VS_OUTPUT
{
    float4 posCS : SV_Position;
    float2 uvs : TEXCOORD;
};


struct RootConstants
{
    unsigned int width;
    unsigned int height;
};


ConstantBuffer<RootConstants> gRootConstants : register(b0, space1);

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

float OutLine(float2 uvcoordinate, const float depthsensitivity, const float normalsensitivity, const float colorsensitivity)
{
    
    /* could implement sobel edgedetection instead? */ 
    
    /* 
    
    -1 0 1 
    -2 0 2
    -1 0 1 
    
    -1 -2 -1
    0 0 0 
    1 2 1
    */
    
    //float2 Texels = 1.0f / float2(gRootConstants.width, gRootConstants.height);
    float2 Texels = 1.0f / float2(1280, 720);
    
    float2 samples[4] =
    {
        uvcoordinate + Texels,
        uvcoordinate - Texels,
        uvcoordinate + float2(Texels.x, -Texels.y),
        uvcoordinate + float2(-Texels.x, Texels.y)
    };
    
    float depthsamples[4];
    float3 normalsamples[4];
    float3 albedosamples[4];
    
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        depthsamples[i] = gGDepthBuffer.Sample(sPointClamp, samples[i]).r;
        normalsamples[i] = gNormalBuffer.Sample(sPointClamp, samples[i]).xyz;
        albedosamples[i] = gAlbedoBuffer.Sample(sPointClamp, samples[i]).xyz;
    }
    
    float depthdiff0 = depthsamples[1] - depthsamples[0];
    float depthdiff1 = depthsamples[3] - depthsamples[2];
    float depthedgevalue = sqrt(depthdiff0 * depthdiff0 + depthdiff1 * depthdiff1);
    float depththreshhold = 1.0f / depthsensitivity;
    depthedgevalue = (depthedgevalue > depththreshhold) ? 1.0f : 0.0f;
        
    float3 normaldiff0 = normalsamples[1] - normalsamples[0];
    float3 normaldiff1 = normalsamples[3] - normalsamples[2];
    float normaledgevalue = sqrt(dot(normaldiff0, normaldiff0) + dot(normaldiff1, normaldiff1));
    float normalthreshold = (1.0f / normalsensitivity);
    normaledgevalue = (normaledgevalue > normalthreshold) ? 1.0f: 0.0f;
    
    float3 albedodiff0 = albedosamples[1] - albedosamples[0];
    float3 albedodiff1 = albedosamples[3] - albedosamples[2];
    float coloredgevalue = sqrt(dot(albedodiff0, albedodiff0) + dot(albedodiff1, albedodiff1));
    float colorthreshhold = (1.0f / colorsensitivity);
    coloredgevalue = (coloredgevalue > colorthreshhold) ? 1.0f : 0.0f;
    
    
    
    float edge = max(depthedgevalue, max(normaledgevalue, coloredgevalue));
    

    return edge;
}


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float3 normal = gNormalBuffer.Sample(sPointClamp, input.uvs).xyz;
    float4 albedo = gAlbedoBuffer.Sample(sPointClamp, input.uvs);
    float3 position = gPositionBuffer.Sample(sPointClamp, input.uvs).xyz;
       
    
    float4 objposcslight = mul(gPassData.lightviewproj, float4(position, 1.0f));
    float shadowfactor =  CalculateShadowFactor(objposcslight);
    float intensity = dot(-(normalize(gPassData.light.direction)), normalize(normal));
   

    float edge = OutLine(input.uvs, 200.0f, 0.35f, 20.0f);
  
    float factor = step(0.0f, intensity);
    
    float4 color = 1.0f;
    
    if (edge)
    {
        color = float4(0.0f, 0.0f, 0.0f, 1.0f);
        //color = intensity * albedo;
    }
    else
    {
        color = (factor * albedo + shadowfactor * albedo) / 2.0f;

    } 

    color.a = 1.0f;
    
    return color;
    
}