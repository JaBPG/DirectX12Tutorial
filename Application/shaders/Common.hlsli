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
    float4x4 lightviewproj;
    LightData light;
};

struct ObjectData
{
    float4x4 transform;
    
};

ConstantBuffer<PassData> gPassData : register(b0);
ConstantBuffer<ObjectData> gObjectData : register(b1);