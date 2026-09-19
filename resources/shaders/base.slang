struct VSInput{
	float2 inPosition;
	float3 inColor;
}

struct UniformBuffer{
	float4x4 model, view, proj;
}
ConstantBuffer<UniformBuffer> ubo;

struct VertextOutput{
	float4 sv_position : SV_Position;
  float3 color;
};

[shader("vertex")]
VertextOutput vertMain(VSInput input){
	VertextOutput output;
	output.sv_position = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition,0.0,1.0))));
  output.color = input.inColor;
	return output;
}

[shader("fragment")]
float4 fragMain(VertextOutput inVert) : SV_Target{
	return float4(inVert.color, 1.);
}
