struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 posCS : SV_Position;
    float2 uvs : TEXCOORD;
};


VS_OUTPUT main(unsigned int vtxIdx : SV_VertexID)
{
    VS_OUTPUT output;
    
    float2 uvs = float2((vtxIdx << 1) & 2, vtxIdx & 2);
 
    output.uvs = uvs;
    
    output.posCS = float4(uvs.x * 2.0f - 1.0f, -uvs.y * 2.0f + 1.0f, 0.0f, 1.0f);
    
    return output;
}