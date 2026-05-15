//Inputs
in vec3 GeoWorldPosition;
in vec3 GeoWorldNormal;
//in vec3 GeoWorldTangent;
//in vec3 GeoWorldBitangent;
in vec2 GeoTexCoord;

//Outputs
out vec4 FragColor;

//Uniforms
uniform vec3 Color;
uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform sampler2D SpecularTexture;

uniform vec3 CameraPosition;

void main()
{
	SurfaceData data;
	data.normal =  normalize(GeoWorldNormal);//SampleNormalMap(NormalTexture, GeoTexCoord, normalize(GeoWorldNormal), normalize(GeoWorldTangent), normalize(GeoWorldBitangent));
	data.albedo = Color * texture(ColorTexture, GeoTexCoord).rgb;
	vec3 arm = texture(SpecularTexture, GeoTexCoord).rgb;
	data.ambientOcclusion = arm.x;
	data.roughness        = arm.y;
	data.metalness        = arm.z;

	vec3 position = GeoWorldPosition;
	vec3 viewDir = GetDirection(position, CameraPosition);
	vec3 color = ComputeLighting(position, data, viewDir, true);
	FragColor = vec4(color.rgb, 1);
}
