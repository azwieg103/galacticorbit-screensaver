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
const float PI = 3.14159265359;
float hash21(vec2 p) { return fract(sin(dot(p, vec2(127.1,311.7))) * 43758.5453); }
float armField(vec2 p, float base, float t, float pitch) {
    float r = max(length(p), 0.002);
    float a = atan(p.y, p.x);
    float spiral = a - base + log(r / 0.035) / pitch + t;
    float wrapped = atan(sin(spiral), cos(spiral));
    float width = mix(0.004, 0.018, smoothstep(0.04, 0.52, r));
    return exp(-pow(wrapped * r / width, 2.0));
}
void main() {
    vec2 pixel = screenOrigin + qt_TexCoord0 * screenSize;
    vec2 uv = (pixel - desktopSize * 0.5) / desktopSize.y;
    float t = phase * 0.105 * rotationSpeed;
    vec3 c = vec3(0.0015,0.0025,0.008);
    float r = length(uv);
    float disk = exp(-pow(uv.y / 0.13, 2.0)) * exp(-pow(r / 0.78, 3.0));
    c += vec3(0.20,0.25,0.42) * disk * 0.22;
    float bulge = exp(-r*r/0.012);
    c += vec3(0.48,0.34,0.22) * bulge * 0.48;
    for (int i=0;i<4;i++) {
        float base = float(i) * (PI*0.5) + 0.35;
        float f = armField(uv,base,t,0.29);
        c += vec3(0.12,0.24,0.48) * f * 0.26;
        float dust = armField(uv,base+0.10,t,0.29);
        c *= 1.0 - dust * 0.13;
    }
    // Orion local spur is a distinct branch; Sun drifts relative to pattern speed.
    float spur = exp(-pow((uv.y - (0.30 + 0.18*uv.x) + 0.10*sin(atan(uv.y,uv.x)-t))/0.010,2.0))
               * smoothstep(0.16,0.28,r) * (1.0-smoothstep(0.54,0.67,r));
    c += vec3(0.24,0.42,0.72) * spur * 0.34;
    float stars = hash21(floor(pixel*0.65));
    float starMask = step(0.9975,stars);
    c += vec3(0.55,0.68,0.95) * starMask * (0.4 + 0.6*hash21(floor(pixel*0.21)));
    // Prominent, intentionally magnified scientific depiction of Sgr A*.
    float diskR = length(vec2(uv.x,uv.y*2.8));
    float accretion = exp(-pow((diskR-0.052)/0.009,2.0));
    c += vec3(1.0,0.36,0.09) * accretion * 1.35;
    c += vec3(0.35,0.58,1.0) * exp(-pow((diskR-0.068)/0.008,2.0)) * 0.35;
    float horizon = smoothstep(0.026,0.023,length(uv));
    c = mix(c,vec3(0.0002,0.0001,0.0004),horizon);
    float photon = exp(-pow((length(uv)-0.029)/0.0028,2.0));
    c += vec3(1.0,0.78,0.46)*photon*0.8;
    // Sun follows a distinct galactic orbital rate, not the spiral pattern.
    float sunAngle = 2.35 - phase*0.098*rotationSpeed;
    vec2 sunPos = 0.34*vec2(cos(sunAngle),sin(sunAngle));
    float sun = length(uv-sunPos);
    c += vec3(1.0,0.36,0.07)*exp(-pow(sun/0.018,2.0))*0.45;
    c += vec3(1.0,0.78,0.38)*exp(-pow(sun/0.006,2.0))*1.4;
    // Planetary system is a restrained set of small, separately orbiting points.
    for (int j=0;j<8;j++) {
        float a = phase*(0.35+float(j)*0.08)+float(j)*1.7;
        float pr = 0.012+float(j)*0.004;
        vec2 pp = sunPos + vec2(cos(a),sin(a))*((0.025+float(j)*0.012));
        float pd = length(uv-pp);
        c += vec3(0.35+0.06*float(j),0.52+0.035*float(j),0.78)*exp(-pow(pd/pr,2.0))*0.5;
    }
    fragColor = vec4(c,1.0)*qt_Opacity;
}
