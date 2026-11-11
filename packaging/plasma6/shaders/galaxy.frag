#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float phase;
    float planetPhase;
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
float hash21(vec2 p) { return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453); }
float noise2(vec2 p) {
    vec2 i=floor(p), f=fract(p);
    f=f*f*(3.0-2.0*f);
    return mix(mix(hash21(i),hash21(i+vec2(1,0)),f.x),
               mix(hash21(i+vec2(0,1)),hash21(i+vec2(1,1)),f.x),f.y);
}
float fbm(vec2 p) {
    // Two octaves preserve broad cloud structure at substantially lower fragment cost.
    float v=0.0, a=0.5;
    for(int i=0;i<2;i++){ v+=noise2(p)*a; p=p*2.03+vec2(17.1,9.2); a*=0.5; }
    return v;
}
vec3 mappedSphere(sampler2D tex, vec2 p) {
    float rr=dot(p,p);
    if(rr>1.0) return vec3(0.0);
    vec3 n=vec3(p.x,p.y,sqrt(max(0.0,1.0-rr)));
    vec2 st=vec2(atan(n.z,n.x)/(2.0*PI)+0.5,asin(clamp(n.y,-1.0,1.0))/PI+0.5);
    vec3 albedo=texture(tex,st).rgb;
    vec3 lightDir=normalize(vec3(-0.42,0.22,0.88));
    float diffuse=0.12+0.88*max(dot(n,lightDir),0.0);
    float limb=smoothstep(0.0,0.18,1.0-rr);
    return albedo*diffuse*limb;
}
vec2 galactic(vec2 uv) {
    float a=-0.20;
    mat2 rot=mat2(cos(a),-sin(a),sin(a),cos(a));
    return rot*vec2(uv.x,uv.y*1.52);
}
vec2 screenFromGalactic(vec2 p) {
    float a=-0.20;
    mat2 inv=mat2(cos(a),sin(a),-sin(a),cos(a));
    vec2 v=inv*p;
    return vec2(v.x,v.y/1.52);
}
float armField(vec2 p,float base,float t,float pitch) {
    float r=max(length(p),0.012);
    float a=atan(p.y,p.x);
    float phaseA=a-base-log(r/0.075)/pitch+t;
    float wrapped=atan(sin(phaseA),cos(phaseA));
    float ridge=wrapped*r;
    float width=mix(0.014,0.045,smoothstep(0.06,0.66,r));
    float turbulent=fbm(p*31.0+vec2(base*4.0,t*0.035));
    float filament=exp(-pow((ridge+0.004*(turbulent-0.5))/width,2.0));
    // Reuse the turbulence sample instead of evaluating another full noise field.
    float clump=0.60+0.40*turbulent;
    return filament*clump;
}
void main() {
    vec2 pixel=screenOrigin+qt_TexCoord0*screenSize;
    vec2 uv=(pixel-desktopSize*0.5)/desktopSize.y;
    vec2 p=galactic(uv);
    float t=phase*0.105*rotationSpeed;
    float r=length(p);
    float edge=1.0-smoothstep(0.66,0.79,r);
    vec3 c=vec3(0.0012,0.0017,0.0055);

    // Diffuse stellar disk and a warm, luminous central bulge.
    float disk=exp(-pow(p.y/0.20,2.0))*exp(-pow(r/0.68,3.0));
    float mottling=0.72+0.28*fbm(p*17.0+vec2(t*0.025,0.0));
    c+=vec3(0.12,0.15,0.27)*disk*mottling*0.62;
    float bulge=exp(-r*r/0.020);
    c+=vec3(0.95,0.48,0.20)*bulge*0.52;
    c+=vec3(0.50,0.40,0.75)*exp(-r*r/0.085)*0.11;

    // Four broad, uneven major arms with blue star clouds and warm H-II knots.
    float armTotal=0.0;
    for(int i=0;i<4;i++) {
        float base=float(i)*(PI*0.5)+0.52+0.11*sin(float(i)*2.17);
        float f=armField(p,base,t,0.40);
        armTotal=max(armTotal,f);
        float cloud=fbm(p*42.0+vec2(float(i)*19.0,t*0.018));
        c+=vec3(0.17,0.29,0.58)*f*(0.42+cloud*0.52);
        c+=vec3(0.12,0.20,0.42)*f*f*0.55;
        float dust=armField(p,base+0.040,t,0.40);
        float dustNoise=0.55+0.45*fbm(p*65.0+vec2(float(i)*11.0,float(i)*5.3));
        c*=1.0-dust*dustNoise*0.34;
        float knots=pow(max(0.0,fbm(p*78.0+vec2(float(i)*31.0,float(i)*13.7)+vec2(t*0.01))-0.48),2.0);
        c+=vec3(0.95,0.48,0.25)*knots*f*1.9;
    }

    // Orion Spur: a distinct shorter branch between two major arms.
    float spurAngle=atan(p.y,p.x);
    float spurPhase=spurAngle-(2.35+log(max(r,0.012)/0.34)/0.40-t);
    float spurDistance=atan(sin(spurPhase),cos(spurPhase))*r;
    float spur=exp(-pow(spurDistance/0.014,2.0))
        *smoothstep(0.20,0.28,r)*(1.0-smoothstep(0.43,0.50,r));
    float spurCloud=fbm(p*53.0+vec2(5.0,t*0.02));
    c+=vec3(0.22,0.39,0.72)*spur*(0.48+spurCloud*0.62);
    c+=vec3(0.92,0.48,0.27)*spur*pow(max(0.0,spurCloud-0.58),2.0)*3.0;

    float field=hash21(floor(pixel*0.72));
    float starMask=step(0.9972,field);
    float starBright=0.35+0.65*hash21(floor(pixel*0.19));
    c+=vec3(0.54,0.66,0.96)*starMask*starBright*(0.35+disk*1.5);
    float fine=hash21(floor(pixel*1.7));
    c+=vec3(0.30,0.39,0.63)*step(0.9992,fine)*disk*0.65;
    c=mix(vec3(0.0012,0.0017,0.0055),c,edge);

    // Sagittarius A*: restore the smaller scale, with tighter ring profiles
    // and stronger local contrast so its structure reads clearly without a label.
    float centerR=length(p);
    float diskR=length(vec2(p.x,p.y*3.5));
    float accretion=exp(-pow((diskR-0.180)/0.022,2.0));
    c+=vec3(1.0,0.34,0.10)*accretion*2.05;
    float accretionCore=exp(-pow((diskR-0.180)/0.009,2.0));
    c+=vec3(1.0,0.78,0.48)*accretionCore*1.10;
    float outerDisk=exp(-pow((diskR-0.255)/0.024,2.0));
    c+=vec3(0.42,0.61,1.0)*outerDisk*0.78;
    float horizon=1.0-smoothstep(0.063,0.0825,centerR);
    c=mix(c,vec3(0.000015,0.000012,0.000035),horizon);
    float photon=exp(-pow((centerR-0.090)/0.0075,2.0));
    c+=vec3(1.0,0.91,0.72)*photon*1.75;
    float photonBlue=exp(-pow((centerR-0.108)/0.010,2.0));
    c+=vec3(0.36,0.58,1.0)*photonBlue*0.62;

    // Keep the Sun exactly on the Orion Spur centerline at its reference radius.
    // Both use the same galactic pattern angle, so the Sun cannot drift off the arm.
    float sunAngle=2.35-t;
    vec2 sunGal=0.34*vec2(cos(sunAngle),sin(sunAngle));
    vec2 sunPos=screenFromGalactic(sunGal);
    float sunDist=length(uv-sunPos);
    c+=vec3(1.0,0.38,0.10)*exp(-pow(sunDist/0.020,2.0))*0.48;
    vec3 sunSurface=mappedSphere(sunTex,(uv-sunPos)/0.0095);
    c=mix(c,c+sunSurface*1.5,step(sunDist,0.0095));

    // The solar system shares one orbital plane inclined 60 degrees to our view.
    // A circular orbit therefore projects to an ellipse with a 0.5 axis ratio.
    const float orbitalInclinationScale=0.5;
    const float orbitalPlaneAngle=0.42;
    float planeCos=cos(orbitalPlaneAngle);
    float planeSin=sin(orbitalPlaneAngle);

    // Draw the eight complete, subtle orbital paths in the same tilted plane.
    vec2 fromSun=uv-sunPos;
    vec2 planeLocal=vec2(planeCos*fromSun.x+planeSin*fromSun.y,
                         -planeSin*fromSun.x+planeCos*fromSun.y);
    float projectedOrbitRadius=length(vec2(planeLocal.x,planeLocal.y/orbitalInclinationScale));
    for(int j=0;j<8;j++) {
        float orbit=0.024+float(j)*0.010;
        float orbitDistance=abs(projectedOrbitRadius-orbit);
        float orbitWidth=0.00024;
        float orbitLine=1.0-smoothstep(orbitWidth-fwidth(orbitDistance),
                                      orbitWidth+fwidth(orbitDistance),orbitDistance);
        c+=mix(vec3(0.12,0.25,0.48),vec3(0.28,0.40,0.62),float(j)/7.0)*orbitLine*0.58;
    }

    // Textured planets follow the same inclined orbital plane; increasing angle
    // preserves the established clockwise motion from the displayed viewpoint.
    for(int j=0;j<8;j++) {
        float a=planetPhase*(0.34+float(j)*0.055)+float(j)*1.7;
        float orbit=0.024+float(j)*0.010;
        vec2 planePoint=vec2(cos(a)*orbit,sin(a)*orbit*orbitalInclinationScale);
        vec2 pp=sunPos+vec2(planeCos*planePoint.x-planeSin*planePoint.y,
                            planeSin*planePoint.x+planeCos*planePoint.y);
        float pr=0.0038+float(j)*0.00048;

        vec2 local=(uv-pp)/pr;
        vec3 world=vec3(0.0);
        if(j==0) world=mappedSphere(mercuryTex,local);
        else if(j==1) world=mappedSphere(venusTex,local);
        else if(j==2) world=mappedSphere(earthTex,local);
        else if(j==3) world=mappedSphere(marsTex,local);
        else if(j==4) world=mappedSphere(jupiterTex,local);
        else if(j==5) world=mappedSphere(saturnTex,local);
        else if(j==6) world=mappedSphere(uranusTex,local);
        else world=mappedSphere(neptuneTex,local);
        c+=world;
    }
    fragColor=vec4(c,1.0)*qt_Opacity;
}
