
layout(triangles) in;
layout(triangle_strip, max_vertices = 256) out;


//worldTangent and bitangent are ignored here to reduce amount of data emitted
in vec3 WorldPosition[];
in vec3 WorldNormal[];
in vec2 TexCoord[];

out vec3 GeoWorldPosition;
out vec3 GeoWorldNormal;
out vec2 GeoTexCoord;

uniform mat4 ViewProjMatrix;
uniform vec3 CameraPosition;

uniform sampler2D PaintTexture;
uniform sampler2D HairTexture;

// seudo deterministic random coutesy of:
// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com
// https://thebookofshaders.com/10/
float random (vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// random point inside current triangle
vec3 getStrandPos(int s, out vec2 uv)
{
    float r1 = random(TexCoord[0].xy + float(s));
    float r2 = random(TexCoord[1].xy + float(s)*2.0);

    float u = r1;
    float v = r2 * (1.0 - u);
    float w = 1.0 - u - v;

    uv =
        u * TexCoord[0] +
        v * TexCoord[1] +
        w * TexCoord[2];

    return
        u * WorldPosition[0] +
        v * WorldPosition[1] +
        w * WorldPosition[2];
}

// interpolated normal
vec3 getNormal(vec2 uv)
{
    return normalize(
        WorldNormal[0] +
        WorldNormal[1] +
        WorldNormal[2]);
}

// cubic bezier
vec3 bezier(vec3 a, vec3 b, vec3 c, vec3 d, float t)
{
    float it = 1.0 - t;
    return it*it*it*a +
           3.0*it*it*t*b +
           3.0*it*t*t*c +
           t*t*t*d;
}

void main()
{
    // pass on the mesh
    for (int i = 0; i < 3; i++)
    {
                
        GeoWorldPosition = WorldPosition[i];
        GeoWorldNormal = WorldNormal[i];
        GeoTexCoord = TexCoord[i];
        gl_Position = gl_in[i].gl_Position;

        EmitVertex();
    }
    EndPrimitive();


    const int strandCount = 3;
    int segments = 8;
    float width = 0.0005;

    for (int s = 0; s < strandCount; s++)
    {
        vec2 uv;
        vec3 pos = getStrandPos(s, uv);

        vec2 growth = texture(PaintTexture, uv).rg;
        vec2 masked = texture(HairTexture, uv).rg;

        if (length(growth * masked) <= 0.0) continue;

        float mask = masked.r;
        float grow = growth.r;

        // random variation
        float r = random(pos.xy + float(s));
        float height = mix(0.36, 0.4, r);

        if (masked.g * growth.g > masked.r * growth.r)
        {
            mask = masked.g;
            grow = growth.g;
        }

        vec3 normal = getNormal(uv);


        vec3 bendDir = normalize(cross(normal, vec3(0,1,0)));

        //bezier control points
     
        vec3 gravity = vec3(0,-1.5,0);

        vec3 p0 = pos;
        vec3 p1 = pos + normal * (height * 0.3);
        vec3 p2 = pos + normalize(normal + gravity * 0.5) * (height * 0.6) + bendDir * 0.05 * r;
        vec3 p3 = pos + normalize(normal + gravity) * height;


        //build strand
        for (int i = 0; i <= segments; i++)
        {
            float t = clamp (float(i) / float(segments), 0.0 , grow * mask);

            vec3 center = bezier(p0, p1, p2, p3, t);

            float dt = 0.01;
            vec3 next = bezier(p0, p1, p2, p3, t + dt);
            vec3 dir = normalize(next - center);

            vec3 viewDir = normalize( center - CameraPosition);
            vec3 side    = normalize(cross(viewDir, dir)) * width;

            vec3 left  = center - side;
            vec3 right = center + side;

            vec3 strandNormal = normalize(cross(side, dir));
            GeoWorldNormal = strandNormal;

            GeoTexCoord = vec2(mix(0.85,0.92,r), 1.0-t);

            GeoWorldPosition = left;
            gl_Position = ViewProjMatrix * vec4(left, 1.0);
            EmitVertex();

            GeoWorldPosition = right;
            gl_Position = ViewProjMatrix * vec4(right, 1.0);


            EmitVertex();
        }

        EndPrimitive();

    };

}