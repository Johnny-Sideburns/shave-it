// strand definition
struct Strand
{
    vec3 p0, p1, p2, p3;
    float seed;
    float grow;
};

// inputs
layout(std430, binding=0) buffer StrandBuffer
{
    Strand strands[];
};

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform vec3 CameraPosition;
uniform int Segments;

// Bernstein version
vec3 bezier(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
    float it = 1.0 - t;
    return it*it*it*a +
           3.0*it*it*t*b +
           3.0*it*t*t*c +
           t*t*t*d;
}

//Outputs
out vec3 WorldPosition;
out vec3 WorldNormal;
out vec3 WorldTangent;
out vec3 WorldBitangent;
out vec2 TexCoord;

void main()
{
    
    int strandID = gl_InstanceID;
    Strand s = strands[strandID];

    int segID = gl_VertexID / 2;
    int side  = gl_VertexID % 2;

    // when tracing along t to get the position of the bezier curve, if t goes beyond the "length" of the hair, it get's clamped
    float t = min(float(segID) / float(Segments), s.grow );

    vec3 center = (WorldMatrix * vec4(bezier(s.p0, s.p1, s.p2, s.p3, t),1)).xyz;
    vec3 next   = (WorldMatrix * vec4(bezier(s.p0, s.p1, s.p2, s.p3, t + 0.01),1)).xyz;

    vec3 dir = normalize(next - center);
 
    vec3 viewDir = normalize(center - CameraPosition);
    
    vec3 c = cross(viewDir, dir);

    //if viewdir and dir are too close default to 1
    if (length(c) < 0.0001)
        c = vec3(1,0,0);

    vec3 right = normalize(c);

    float width = 0.0008;

    float offset = (side == 0 ? -1.0 : 1.0) * width;

    vec3 worldpos = center + right * offset;

    WorldTangent = dir;
    WorldBitangent = right;
    WorldPosition = worldpos;
    WorldNormal = normalize(cross(right,  dir));
    TexCoord = vec2(mix(0.8,0.9, s.seed), 1-t);
    gl_Position = ViewProjMatrix * vec4(worldpos, 1.0);
}