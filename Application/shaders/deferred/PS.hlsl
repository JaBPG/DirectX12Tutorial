struct VS_OUTPUT
{
	
    float4 positionCS : SV_POSITION;
    float3 positionWS : POSITION0;
    float3 normal : NORMAL;
	
};

struct PS_OUTPUT
{
    float4 position : SV_Target0; //last component could be objectidx (w)
    float4 normal : SV_Target1; //last component (w) could be material idx
    float4 albedocolor : SV_Target2;

};


struct MaterialData
{
    float4 diffuseAlbedo;
    
};

ConstantBuffer<MaterialData> gMaterialData : register(b2);

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    output.position = float4(input.positionWS, 1.0f);
    output.normal = float4(input.normal, 1.0f);
    output.albedocolor = gMaterialData.diffuseAlbedo;
    
 
    return output;
}