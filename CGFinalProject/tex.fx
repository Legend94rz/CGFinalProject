#include "lighthelper.fx"
 
cbuffer cbPerFrame
{
	Light gLight;
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP; 
	float4x4 gTexMtx;
};

Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
		
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	vOut.texC  = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);
	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target
{
	float4 diffuse = gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
	float4 spec    = gSpecMap.Sample( gTriLinearSam, pIn.texC );
	
	spec.a *= 256.0f;
	
    float3 normalW = normalize(pIn.normalW);
    
    SurfaceInfo v = {pIn.posW, normalW, diffuse, spec};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);
    return float4(litColor, diffuse.a);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
