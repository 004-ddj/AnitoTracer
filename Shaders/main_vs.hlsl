#include "common_struct.hlsli"

void main_vs(in VertexInput In, out PSInput Out)
{
    float4 worldPos = mul(float4(In.Pos, 1.0), g_Model);
    float4 viewPos = mul(worldPos, g_View);
    Out.Pos = mul(viewPos, g_Proj);
    
    Out.WorldPos = worldPos.xyz;
    Out.Normal = normalize(mul(In.Norm, (float3x3)g_Model));
    Out.Tangent = normalize(mul(In.Tangent, (float3x3)g_Model));
    Out.Bitangent = normalize(mul(In.Bitangent, (float3x3)g_Model));
    Out.UV = In.uv;
}