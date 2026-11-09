#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float phase;
    vec2 desktopSize;
    vec2 screenOrigin;
    vec2 screenSize;
    float rotationSpeed;
    float showLabels;
};
layout(binding = 1) uniform sampler2D sunTex;
layout(binding = 2) uniform sampler2D mercuryTex;
layout(binding = 3) uniform sampler2D venusTex;
layout(binding = 4) uniform sampler2D earthTex;
layout(binding = 5) uniform sampler2D marsTex;
layout(binding = 6) uniform sampler2D jupiterTex;
layout(binding = 7) uniform sampler2D saturnTex;
layout(binding = 8) uniform sampler2D uranusTex;
layout(binding = 9) uniform sampler2D neptuneTex;
const float PI = 3.14159265359;
float hash21(vec2 p) { return fract(sin(dot(p, vec2(127.1,311.7))) * 43758.5453); }
vec3 mappedSphere(sampler2D tex, vec2 p) {
    float rr = dot(p,p);
    if (rr > 1.0) return vec3(0.0);
    vec3 n = vec3(p.x, p.y, sqrt(max(0.0,1.0-rr)));
    vec2 st = vec2(atan(n.z,n.x)/(2.0*PI)+0.5, asin(clamp(n.y,-1.0,1.0))/PI+0.5);
    vec3 albedo = texture(tex,st).rgb;
    vec3 lightDir = normalize(vec3(-0.42,0.22,0.88));
    float diffuse = 0.12 + 0.88*max(dot(n,lightDir),0.0);
    float limb = smoothstep(0.0,0.18,1.0-rr);
    return albedo * diffuse * limb;
}
float armField(vec2 p, float base, float t, float pitch) {
    float r = max(length(p), 0.002);
    float a = atan(p.y, p.x);
    // Logarithmic spiral pattern, rotating as one coherent galactic structure.
    float spiral = a - base - log(r / 0.035) / pitch + t;
    float wrapped = atan(sin(spiral), cos(spiral));
    float width = mix(0.006, 0.025, smoothstep(0.035, 0.45, r));
    return exp(-pow(wrapped * r / width, 2.0));
}
void main() {
    vec2 pixel = screenOrigin + qt_TexCoord0 * screenSize;
    vec2 uv = (pixel - desktopSize * 0.5) / desktopSize.y;
    float t = phase * 0.105 * rotationSpeed;
    vec3 c = vec3(0.0015,0.0025,0.008);
    float r = length(uv);
    // The complete face-on disk is framed inside the combined 2x2 display.
    // Coordinates are normalized by desktop height, so the disk radius is ~0.44.
    float disk = exp(-pow(uv.y / 0.095, 2.0)) * exp(-pow(r / 0.47, 4.0));
    c += vec3(0.20,0.25,0.42) * disk * 0.30;
    float bulge = exp(-r*r/0.0045);
    c += vec3(0.48,0.34,0.22) * bulge * 0.52;
    for (int i=0;i<4;i++) {
        float base = float(i) * (PI*0.5) + 0.35;
        float f = armField(uv,base,t,0.29);
        c += vec3(0.15,0.29,0.58) * f * 0.48;
        c += vec3(0.20,0.28,0.44) * f * 0.22;
        float dust = armField(uv,base+0.075,t,0.29);
        c *= 1.0 - dust * 0.18;
    }
    // Orion Spur branches between two major arms. Its reference radius is the Sun's.
    float spurSpiral = spurAngle - (2.35 + log(max(r,0.002)/0.34)/0.29 - t);
    float spur = exp(-pow(atan(sin(spurSpiral),cos(spurSpiral))*r/0.012,2.0))
               * smoothstep(0.22,0.29,r) * (1.0-smoothstep(0.40,0.47,r));
    c += vec3(0.30,0.52,0.82) * spur * 0.58;
    float stars = hash21(floor(pixel*0.65));
    float starMask = step(0.9975,stars);
    c += vec3(0.55,0.68,0.95) * starMask * (0.4 + 0.6*hash21(floor(pixel*0.21)));
    // Sagittarius A*: a compact central black hole, tiny beside the galactic disk.
    float diskR = length(vec2(uv.x,uv.y*3.2));
    float accretion = exp(-pow((diskR-0.025)/0.0045,2.0));
    c += vec3(1.0,0.39,0.12) * accretion * 1.65;
    c += vec3(0.42,0.62,1.0) * exp(-pow((diskR-0.034)/0.004,2.0)) * 0.42;
    float horizon = 1.0 - smoothstep(0.0085,0.0105,length(uv));
    c = mix(c,vec3(0.0001,0.0001,0.0003),horizon);
    float photon = exp(-pow((length(uv)-0.012)/0.0014,2.0));
    c += vec3(1.0,0.82,0.55)*photon*0.95;
    // Sun remains at r=0.34 on the Orion Spur, drifting slightly relative to its pattern.
    float sunAngle = 2.35 - t + phase*0.010*rotationSpeed;
    vec2 sunPos = 0.34*vec2(cos(sunAngle),sin(sunAngle));
    float sun = length(uv-sunPos);
    c += vec3(1.0,0.36,0.07)*exp(-pow(sun/0.024,2.0))*0.45;
    vec3 sunSurface = mappedSphere(sunTex,(uv-sunPos)/0.010);
    c = mix(c,c+sunSurface*1.35,step(sun,0.010));
    // Textured spherical worlds follow separate, faster heliocentric orbits.
    for (int j=0;j<8;j++) {
        float a = phase*(0.34+float(j)*0.055)+float(j)*1.7;
        float orbit = 0.025+float(j)*0.011;
        vec2 pp = sunPos + vec2(cos(a),sin(a))*orbit;
        float pr = 0.0048+float(j)*0.00055;
        vec2 local = (uv-pp)/pr;
        vec3 world = vec3(0.0);
        if (j==0) world = mappedSphere(mercuryTex,local);
        else if (j==1) world = mappedSphere(venusTex,local);
        else if (j==2) world = mappedSphere(earthTex,local);
        else if (j==3) world = mappedSphere(marsTex,local);
        else if (j==4) world = mappedSphere(jupiterTex,local);
        else if (j==5) world = mappedSphere(saturnTex,local);
        else if (j==6) world = mappedSphere(uranusTex,local);
        else world = mappedSphere(neptuneTex,local);
        c += world;
    }
    fragColor = vec4(c,1.0)*qt_Opacity;
}
