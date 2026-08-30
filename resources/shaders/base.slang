struct VSInput{
	float2 inPosition;
	float3 inColor;
}

struct VertextOutput{
	float4 sv_position : SV_Position;
  float3 color;
};

[shader("vertex")]
VertextOutput vertMain(VSInput input){
	VertextOutput output;
	output.sv_position = float4(input.inPosition,0.0,1.0);
  output.color = input.inColor;
	return output;
}

[shader("fragment")]
float4 fragMain(VertextOutput inVert) : SV_Target{
  float3 color = inVert.color;
	return float4(color, 1.);
}
