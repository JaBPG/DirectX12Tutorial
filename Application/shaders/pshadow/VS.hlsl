struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    
};


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
  
};

struct LightData
{
    float3 position;
    float strenght;
    float3 direction;
    float padding;

};

struct PassData
{
    float4x4 viewproj;
    LightData light;
};


struct ObjectData
{
    float4x4 transform;
    
};

ConstantBuffer<PassData> gPassData : register(b0);
ConstantBuffer<ObjectData> gObjectData : register(b1);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    
    float4 worldProjectPos = mul(gObjectData.transform, float4(input.position, 1.0f));
    

    output.position = mul(gPassData.viewproj, worldProjectPos);
    
    
    return output;
}
