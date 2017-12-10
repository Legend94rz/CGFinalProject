matrix gWVP;
TextureCube gCubeMap;
SamplerState gLinearSam
{
	Filter=MIN_MAG_MIP_LINEAR;
	AddressU=Wrap;
	AddressV=Wrap;
};
struct VS_IN
{
	float3 posL : POSITION;
};
struct VS_OUT
{
	float4 posH: SV_POSITION;
	float3 texC: TEXCOORD;
};
//输入输出用结构体吧
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.posH=mul(float4(vIn.posL,1),gWVP).xyww;
	vOut.texC=vIn.posL;
	return vOut;
}
float4 PS(VS_OUT pIn):SV_Target
{
	return gCubeMap.Sample(gLinearSam,pIn.texC);
}
RasterizerState NoCull
{
	CullMode=None;
};
DepthStencilState LessEqualDSS
{
	DepthFunc=LESS_EQUAL;
};
technique10 SkyTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0,VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0,PS()));

		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS,0);
	}
}
