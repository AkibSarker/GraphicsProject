#define GL_SILENCE_DEPRECATION
#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <ctime>

#define PI 3.14159265359f

using namespace std;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

int currentPage = 1;
int selectedPlanet = -1;

GLfloat mercuryAngle = 0.0f;
GLfloat venusAngle = 0.0f;
GLfloat earthAngle = 0.0f;
GLfloat marsAngle = 0.0f;
GLfloat jupiterAngle = 0.0f;
GLfloat saturnAngle = 0.0f;
GLfloat uranusAngle = 0.0f;
GLfloat neptuneAngle = 0.0f;
GLfloat moonAngle = 0.0f;
GLfloat speed = 0.2f;
GLfloat targetSpeed = 0.2f;
GLfloat worldScale = 1.0f;
GLfloat angleAll = 0.0f;
GLfloat meteorX = -1.0f;
GLfloat meteorY = 0.8f;
bool showMeteor = false;
int rotateFlag = 1;

struct StarOrig {
    float x, y, brightness, twinkleSpeed;
};

struct Asteroid {
    float angle, distance, size, speed, offsets[6];
};

struct PlanetInfo {
    const char* name;
    const char* keyText;
    const char* info1;
    const char* info2;
    float x, y, radius;
    int r, g, b;
    bool hasRing;
};

vector<StarOrig> starsOrig;
vector<Asteroid> asteroidBelt;

PlanetInfo galleryPlanets[8] = {
    {"MERCURY", "[A]", "Smallest planet", "Closest to the Sun", -0.75f,  0.42f, 0.035f, 170,170,170, false},
    {"VENUS",   "[B]", "Hottest planet",  "Thick atmosphere",   -0.25f,  0.42f, 0.050f, 230,190,110, false},
    {"EARTH",   "[C]", "Our home planet", "1 natural moon",       0.25f,  0.42f, 0.052f,   0,150,255, false},
    {"MARS",    "[D]", "The red planet",  "2 small moons",        0.75f,  0.42f, 0.045f, 255, 80, 40, false},
    {"JUPITER", "[E]", "Largest planet",  "Giant red spot",      -0.75f, -0.35f, 0.095f, 218,175,125, false},
    {"SATURN",  "[F]", "Famous rings",    "Gas giant planet",    -0.25f, -0.35f, 0.085f, 235,205,120, true},
    {"URANUS",  "[G]", "Tilted ice giant","Blue-green planet",    0.25f, -0.35f, 0.070f, 160,235,235, false},
    {"NEPTUNE", "[H]", "Farthest planet", "Strong blue storms",   0.75f, -0.35f, 0.070f,  75, 95,220, false}
};

void drawCircle(float x, float y, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= seg; i++) {
        float theta = 2.0f * PI * i / seg;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}

void drawBigText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawGlow(float x, float y, float r, int R, int G, int B) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 15; i++) {
        float ratio = (float)i / 15.0f;
        float currentR = r * (1.0f + ratio * 1.8f);
        float alpha = (1.0f - ratio) * 0.28f;
        glColor4f(R/255.0f, G/255.0f, B/255.0f, alpha);
        drawCircle(x, y, currentR, 80);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initStarsOrig() {
    starsOrig.clear();
    for (int i = 0; i < 600; i++) {
        StarOrig s;
        s.x = (rand() % 240 - 120) / 100.0f;
        s.y = (rand() % 200 - 100) / 100.0f;
        s.brightness = 0.25f + (rand() % 75) / 100.0f;
        s.twinkleSpeed = 0.4f + (rand() % 12) / 10.0f;
        starsOrig.push_back(s);
    }
}

void drawStarsOrig() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (size_t i = 0; i < starsOrig.size(); i++) {
        StarOrig s = starsOrig[i];
        float twinkle = 0.35f + 0.65f * sin(angleAll * s.twinkleSpeed + s.x * 20.0f);
        float b = s.brightness * twinkle;
        glColor4f(b, b, b * 1.15f, b);
        glPointSize(1.0f + b * 1.8f);
        glBegin(GL_POINTS);
        glVertex2f(s.x, s.y);
        glEnd();
    }
}

void initAsteroids() {
    asteroidBelt.clear();
    for (int i = 0; i < 500; i++) {
        Asteroid a;
        a.angle = (rand() % 360);
        a.distance = 0.60f + (rand() % 100 / 1000.0f);
        a.size = 0.0005f + (rand() % 10 / 10000.0f);
        a.speed = (rand() % 50 + 10) * 0.01f;
        for (int j = 0; j < 6; j++) {
            a.offsets[j] = (rand() % 100 / 50000.0f);
        }
        asteroidBelt.push_back(a);
    }
}

void drawAsteroids() {
    for (size_t k = 0; k < asteroidBelt.size(); k++) {
        Asteroid a = asteroidBelt[k];
        glPushMatrix();
        glRotatef(a.angle + angleAll * a.speed * speed * 5.0f, 0, 0, 1);
        glTranslatef(a.distance, 0, 0);
        glColor3ub(170,170,180);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 6; i++) {
            float theta = 2.0f * PI * i / 6.0f;
            float r = a.size + a.offsets[i];
            glVertex2f(r * cos(theta), r * sin(theta));
        }
        glEnd();
        glPopMatrix();
    }
}

void drawOrbit(float radius) {
    glColor4f(0.5f,0.6f,0.8f,0.15f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 200; i++) {
        float a = 2.0f * PI * i / 200.0f;
        glVertex2f(radius * cos(a), radius * sin(a));
    }
    glEnd();
}

void drawSunSpots() {
    glColor3ub(0,0,0);
    glPushMatrix();
    glTranslatef(0.04f,0.05f,0);
    drawCircle(0,0,0.012f,15);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.05f,0.02f,0);
    drawCircle(0,0,0.010f,15);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.01f,-0.06f,0);
    drawCircle(0,0,0.013f,15);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.02f,-0.02f,0);
    drawCircle(0,0,0.007f,15);
    glPopMatrix();
}

void drawShootingStar() {
    if (!showMeteor) return;
    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);
    glVertex2f(meteorX, meteorY);
    glColor4f(0.5f,0.7f,1.0f,0);
    glVertex2f(meteorX-0.15f, meteorY+0.05f);
    glVertex2f(meteorX-0.12f, meteorY+0.08f);
    glEnd();
    glColor3ub(255,255,255);
    drawCircle(meteorX, meteorY, 0.005f, 40);
}

void drawSaturnRingSmall(float cx, float cy) {
    glColor4f(0.8f,0.7f,0.4f,0.5f);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    for (int i=0;i<200;i++) {
        float a=2*PI*i/200;
        glVertex2f(cx+0.10f*cos(a), cy+0.04f*sin(a));
    }
    glEnd();
    glColor4f(0.6f,0.5f,0.3f,0.4f);
    glBegin(GL_LINE_LOOP);
    for (int i=0;i<200;i++) {
        float a=2*PI*i/200;
        glVertex2f(cx+0.07f*cos(a), cy+0.03f*sin(a));
    }
    glEnd();
}

void drawPlanetMasterLook(float distance, float angle, float radius, int r,int g,int b, const char* name) {
    glPushMatrix();
    glRotatef(angle,0,0,1);
    glTranslatef(distance,0,0);
    drawGlow(0,0,radius,r,g,b);
    if (strcmp(name,"SATURN")==0) {
        glPushMatrix();
        glRotatef(-18,0,0,1);
        drawSaturnRingSmall(0,0);
        glPopMatrix();
    }
    glColor3ub(r,g,b);
    drawCircle(0,0,radius,100);
    if (strcmp(name,"EARTH")==0) {
        glColor3ub(50,180,50);
        glPushMatrix();
        glTranslatef(-radius*0.2f, -radius*0.1f,0);
        glRotatef(30,0,0,1);
        glBegin(GL_POLYGON);
        for (int i=0;i<120;i+=2) {
            float theta=i*PI/180;
            glVertex2f(radius*0.6f*cos(theta), radius*0.5f*sin(theta));
        }
        glVertex2f(0,0);
        glEnd();
        glPopMatrix();
    }
    if (strcmp(name,"JUPITER")==0) {
        glColor3ub(225,75,50);
        drawCircle(radius*0.35f, -radius*0.15f, radius*0.15f,35);
    }
    glColor4f(0,0,0,0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for (int i=-90;i<=90;i++) {
        float theta=i*PI/180;
        glVertex2f(radius*cos(theta), radius*sin(theta));
    }
    glEnd();
    float textWidth = strlen(name)*0.012f;
    glColor3ub(255,255,255);
    drawText(-textWidth/2, radius+0.025f, name);
    glPopMatrix();
}

void drawMoon(float distance, float angle, float radius) {
    glPushMatrix();
    glRotatef(angle,0,0,1);
    glTranslatef(distance,0,0);
    drawGlow(0,0,radius,200,200,200);
    glColor3ub(200,200,200);
    drawCircle(0,0,radius,50);
    glPopMatrix();
}

void drawPage1SolarSystem() {
    glScalef(worldScale, worldScale, 1);
    drawStarsOrig();
    drawAsteroids();
    drawShootingStar();
    drawOrbit(0.25f);
    drawOrbit(0.35f);
    drawOrbit(0.45f);
    drawOrbit(0.55f);
    drawOrbit(0.70f);
    drawOrbit(0.85f);
    drawOrbit(1.00f);
    drawOrbit(1.15f);
    drawGlow(0,0,0.15f,255,180,50);
    glColor3ub(255,255,0);
    drawCircle(0,0,0.15f,100);
    drawSunSpots();
    glColor3ub(255,255,255);
    drawText(-0.03f,0.18f,"SUN");
    drawPlanetMasterLook(0.25f, mercuryAngle, 0.03f, 169,169,169, "MERCURY");
    drawPlanetMasterLook(0.35f, venusAngle, 0.04f, 220,180,120, "VENUS");
    glPushMatrix();
    glRotatef(earthAngle,0,0,1);
    glTranslatef(0.45f,0,0);
    drawPlanetMasterLook(0,0,0.045f,0,150,255,"EARTH");
    drawMoon(0.08f, moonAngle, 0.012f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(marsAngle,0,0,1);
    glTranslatef(0.55f,0,0);
    drawPlanetMasterLook(0,0,0.04f,255,80,40,"MARS");
    drawMoon(0.06f, moonAngle, 0.008f);
    drawMoon(0.08f, moonAngle+120, 0.007f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(jupiterAngle,0,0,1);
    glTranslatef(0.70f,0,0);
    drawPlanetMasterLook(0,0,0.07f,210,180,140,"JUPITER");
    drawMoon(0.10f, moonAngle, 0.010f);
    drawMoon(0.13f, moonAngle+60, 0.012f);
    drawMoon(0.16f, moonAngle+120, 0.009f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(saturnAngle,0,0,1);
    glTranslatef(0.85f,0,0);
    drawPlanetMasterLook(0,0,0.06f,255,210,120,"SATURN");
    drawMoon(0.10f, moonAngle, 0.010f);
    drawMoon(0.13f, moonAngle+60, 0.009f);
    drawMoon(0.16f, moonAngle+120, 0.008f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(uranusAngle,0,0,1);
    glTranslatef(1.00f,0,0);
    drawPlanetMasterLook(0,0,0.05f,175,238,238,"URANUS");
    drawMoon(0.08f, moonAngle, 0.008f);
    drawMoon(0.11f, moonAngle+90, 0.007f);
    glPopMatrix();
    drawPlanetMasterLook(1.15f, neptuneAngle, 0.05f, 72,61,139, "NEPTUNE");
    glColor3ub(230,230,245);
    drawText(-1.15f,0.92f,"PAGE 1: Solar System   |   2: Planet Selection   3: Earth/Moon + Map   4: Day-Night   5: Village");
}

void drawRing(float rx, float ry) {
    glColor4f(0.85f,0.75f,0.45f,0.55f);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for (int i=0;i<180;i++) {
        float a=2*PI*i/180;
        glVertex2f(rx*cos(a), ry*sin(a));
    }
    glEnd();
    glColor4f(0.45f,0.35f,0.20f,0.45f);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
    for (int i=0;i<180;i++) {
        float a=2*PI*i/180;
        glVertex2f(rx*0.72f*cos(a), ry*0.72f*sin(a));
    }
    glEnd();
}

void drawPlanetBody(const PlanetInfo& p, float radius, bool detailed) {
    drawGlow(0,0,radius,p.r,p.g,p.b);
    if(p.hasRing) {
        glPushMatrix();
        glRotatef(-18,0,0,1);
        drawRing(radius*1.8f, radius*0.58f);
        glPopMatrix();
    }
    glColor3ub(p.r,p.g,p.b);
    drawCircle(0,0,radius,100);
    if(strcmp(p.name,"EARTH")==0) {
        glColor3ub(40,175,70);
        glPushMatrix();
        glRotatef(angleAll*35,0,0,1);
        glTranslatef(-radius*0.20f, radius*0.05f,0);
        drawCircle(0,0,radius*0.32f,40);
        glTranslatef(radius*0.45f, -radius*0.28f,0);
        drawCircle(0,0,radius*0.24f,40);
        glPopMatrix();
    }
    if(strcmp(p.name,"JUPITER")==0) {
        glColor4f(0.95f,0.80f,0.55f,0.65f);
        for(int i=-2;i<=2;i++) {
            glBegin(GL_LINES);
            glVertex2f(-radius*0.8f, i*radius*0.22f);
            glVertex2f(radius*0.8f, i*radius*0.22f);
            glEnd();
        }
        glColor3ub(225,75,50);
        drawCircle(radius*0.35f, -radius*0.15f, radius*0.16f,40);
    }

        glEnd();
    }


void drawMoonsAround(float radius, int count) {
    for(int i=0;i<count;i++) {
        glPushMatrix();
        glRotatef(angleAll*(45+i*20)+i*90,0,0,1);
        glTranslatef(radius*(1.65f+i*0.30f),0,0);
        drawGlow(0,0,radius*0.08f,200,200,220);
        glColor3ub(210,210,220);
        drawCircle(0,0,radius*0.08f,30);
        glPopMatrix();
    }
}

void drawGalleryPlanet(int index) {
    PlanetInfo p = galleryPlanets[index];
    glPushMatrix();
    glTranslatef(p.x,p.y,0);
    glPushMatrix();
    glRotatef(angleAll*28,0,0,1);
    drawPlanetBody(p, p.radius, false);
    glPopMatrix();
    if(index==2) drawMoonsAround(p.radius,1);
    if(index==3) drawMoonsAround(p.radius,2);
    if(index==4) drawMoonsAround(p.radius,4);
    if(index==5) drawMoonsAround(p.radius,3);
    if(index==6) drawMoonsAround(p.radius,2);
    if(index==7) drawMoonsAround(p.radius,1);
    glColor3ub(220,220,240);
    drawText(-0.055f, p.radius+0.12f, p.name);
    drawText(-0.015f, -p.radius-0.12f, p.keyText);
    glPopMatrix();
}

void drawGalleryScene() {
    glColor3ub(230,230,245);
    drawBigText(-0.34f,0.80f,"SELECT A PLANET (A-H) TO ZOOM IN");
    glColor3ub(210,210,225);
    drawText(-0.43f,0.70f,"Press Z to return   |   1: Solar System   3: Earth/Moon + Map   4: Day-Night   5: Village");
    for(int i=0;i<8;i++) drawGalleryPlanet(i);
}

void drawOrbitLine(float radius) {
    glColor4f(0.6f,0.7f,1.0f,0.25f);
    glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<180;i++) {
        float a=2*PI*i/180;
        glVertex2f(radius*cos(a), radius*sin(a));
    }
    glEnd();
}

void drawZoomScene() {
    PlanetInfo p = galleryPlanets[selectedPlanet];
    glColor3ub(235,235,250);
    drawBigText(-0.10f,0.80f,p.name);
    glColor3ub(210,210,225);
    drawText(-0.42f,0.70f,"Press Z to go back   |   A-H: another planet   1/3/4/5: other pages");
    glPushMatrix();
    glTranslatef(0,-0.05f,0);
    drawOrbitLine(0.42f);
    drawOrbitLine(0.56f);
    glPushMatrix();
    glScalef(1,1,1);
    glRotatef(angleAll*22,0,0,1);
    drawPlanetBody(p,0.22f,true);
    glPopMatrix();
    if(selectedPlanet==2) drawMoonsAround(0.22f,1);
    if(selectedPlanet==3) drawMoonsAround(0.22f,2);
    if(selectedPlanet==4) drawMoonsAround(0.22f,4);
    if(selectedPlanet==5) drawMoonsAround(0.22f,3);
    if(selectedPlanet==6) drawMoonsAround(0.22f,2);
    if(selectedPlanet==7) drawMoonsAround(0.22f,1);
    glPopMatrix();
    glColor3ub(245,245,255);
    drawBigText(-0.95f,-0.62f,"PLANET DETAILS");
    glColor3ub(210,220,245);
    drawText(-0.95f,-0.71f,p.info1);
    drawText(-0.95f,-0.78f,p.info2);

}

void drawPage2PlanetSelection() {
    drawStarsOrig();
    if(selectedPlanet==-1) drawGalleryScene();
    else drawZoomScene();
}

namespace Ftask3 {
const int SCR_W = 1000;
const int SCR_H = 520;
float earthSpin=0;
float moonOrbit=0;
float speedValue=1.0f;
int sceneMode=3;
float mapScale=1.0f;
bool running=true;

struct StarF { float x,y,brightness; };
vector<StarF> starsF;

void initStarsF() {
    starsF.clear();
    for(int i=0;i<350;i++) {
        StarF s;
        s.x=(rand()%240-120)/100.0f;
        s.y=(rand()%200-100)/100.0f;
        s.brightness=0.25f+(rand()%75)/100.0f;
        starsF.push_back(s);
    }
}

void drawStarsF() {
    glPointSize(1.2f);
    glBegin(GL_POINTS);
    for(size_t i=0;i<starsF.size();i++) {
        float b=starsF[i].brightness;
        glColor3f(b,b,b);
        glVertex2f(starsF[i].x, starsF[i].y);
    }
    glEnd();
}

void drawRectF(float x1,float y1,float x2,float y2) {
    drawRect(x1,y1,x2,y2);
}

void drawEllipse(float x,float y,float rx,float ry,int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x,y);
    for(int i=0;i<=seg;i++) {
        float theta=2*PI*i/seg;
        glVertex2f(x+rx*cos(theta), y+ry*sin(theta));
    }
    glEnd();
}

void drawGlowF(float x,float y,float r,int red,int green,int blue) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for(int i=0;i<18;i++) {
        float ratio=i/18.0f;
        float currR=r*(1+ratio*2.5f);
        float alpha=(1-ratio)*0.28f;
        glColor4f(red/255.0f, green/255.0f, blue/255.0f, alpha);
        drawCircle(x,y,currR,90);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawSun() {
    glPushMatrix();
    glTranslatef(-0.48f,0.62f,0);
    drawGlowF(0,0,0.13f,255,210,60);
    drawGlowF(0,0,0.09f,255,255,180);
    glColor3ub(255,255,210);
    drawCircle(0,0,0.125f,100);
    glColor3ub(255,230,80);
    drawCircle(0,0,0.035f,60);
    glPopMatrix();
}

void drawLensFlare() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    float x=-0.05f, y=0.66f;
    for(int i=0;i<5;i++) {
        float r=0.035f+i*0.012f;
        glColor4f(0.45f,0.05f,0.45f,0.08f);
        drawCircle(x+i*0.08f, y-i*0.025f, r,50);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

float getDarkSideAngle(float bodyX,float bodyY) {
    float sunX=-0.48f, sunY=0.62f;
    float darkX=bodyX-sunX, darkY=bodyY-sunY;
    return atan2(darkY,darkX)*180/PI;
}

void drawDirectionalShadow(float radius, float darkAngleDeg, float alpha) {
    glColor4f(0,0,0,alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for(int i=-90;i<=90;i++) {
        float theta=(darkAngleDeg+i)*PI/180;
        glVertex2f(radius*cos(theta), radius*sin(theta));
    }
    glEnd();
}

void drawEarth() {
    drawGlowF(0,0,0.17f,40,130,255);
    glColor3ub(40,95,215);
    drawCircle(0,0,0.16f,120);
    glPushMatrix();
    glRotatef(earthSpin,0,0,1);
    glColor3ub(45,155,65);
    glBegin(GL_POLYGON);
    glVertex2f(-0.07f,0.04f);
    glVertex2f(-0.02f,0.10f);
    glVertex2f(0.03f,0.07f);
    glVertex2f(0.01f,0.01f);
    glVertex2f(-0.05f,-0.02f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.04f,0.08f);
    glVertex2f(0.11f,0.12f);
    glVertex2f(0.13f,0.04f);
    glVertex2f(0.08f,-0.01f);
    glVertex2f(0.03f,0.02f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-0.02f,-0.02f);
    glVertex2f(0.03f,-0.08f);
    glVertex2f(0.01f,-0.14f);
    glVertex2f(-0.06f,-0.10f);
    glVertex2f(-0.08f,-0.04f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.06f,-0.07f);
    glVertex2f(0.11f,-0.09f);
    glVertex2f(0.12f,-0.12f);
    glVertex2f(0.08f,-0.14f);
    glVertex2f(0.04f,-0.11f);
    glEnd();
    glPopMatrix();
    glColor4f(0.65f,0.85f,1.0f,0.45f);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<140;i++) {
        float theta=2*PI*i/140;
        glVertex2f(0.162f*cos(theta), 0.162f*sin(theta));
    }
    glEnd();
    drawDirectionalShadow(0.16f, getDarkSideAngle(0,-0.08f), 0.38f);
}

void drawMoonBody(float localShadowAngle) {
    drawGlowF(0,0,0.045f,210,215,235);
    glColor3ub(205,208,218);
    drawCircle(0,0,0.045f,70);
    glColor3ub(150,155,170);
    drawCircle(-0.014f,0.012f,0.009f,25);
    drawCircle(0.010f,-0.007f,0.006f,25);
    drawCircle(-0.004f,-0.020f,0.005f,25);
    drawDirectionalShadow(0.045f, localShadowAngle, 0.32f);
}

void drawEarthMoonSystem() {
    glPushMatrix();
    glTranslatef(0,-0.08f,0);
    glColor4f(0.7f,0.8f,1.0f,0.18f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<220;i++) {
        float theta=2*PI*i/220;
        glVertex2f(0.31f*cos(theta), 0.31f*sin(theta));
    }
    glEnd();
    drawEarth();
    float moonOrbitRadius=0.31f, moonAngleRad=moonOrbit*PI/180;
    float earthWorldX=0, earthWorldY=-0.08f;
    float moonWorldX=earthWorldX+moonOrbitRadius*cos(moonAngleRad);
    float moonWorldY=earthWorldY+moonOrbitRadius*sin(moonAngleRad);
    float moonWorldShadowAngle=getDarkSideAngle(moonWorldX,moonWorldY);
    float moonLocalShadowAngle=moonWorldShadowAngle-moonOrbit;
    glPushMatrix();
    glRotatef(moonOrbit,0,0,1);
    glTranslatef(moonOrbitRadius,0,0);
    drawMoonBody(moonLocalShadowAngle);
    glPopMatrix();
    glPopMatrix();
}

void drawMapLandShape(float shade) {
    glColor3f(0.12f*shade,0.62f*shade,0.18f*shade);
    glBegin(GL_POLYGON); // N.America
    glVertex2f(-1.72f,0.66f);
    glVertex2f(-1.42f,0.80f);
    glVertex2f(-1.08f,0.72f);
    glVertex2f(-0.90f,0.52f);
    glVertex2f(-1.00f,0.32f);
    glVertex2f(-1.18f,0.18f);
    glVertex2f(-1.08f,0.04f);
    glVertex2f(-1.34f,0.10f);
    glVertex2f(-1.56f,0.28f);
    glEnd();
    glColor3f(0.13f*shade,0.58f*shade,0.18f*shade);
    glBegin(GL_POLYGON); // Greenland
    glVertex2f(-0.94f,0.88f);
    glVertex2f(-0.62f,0.98f);
    glVertex2f(-0.42f,0.84f);
    glVertex2f(-0.56f,0.68f);
    glVertex2f(-0.88f,0.70f);
    glEnd();
    glColor3f(0.10f*shade,0.58f*shade,0.17f*shade);
    glBegin(GL_POLYGON); // S.America
    glVertex2f(-1.10f,0.02f);
    glVertex2f(-0.84f,-0.08f);
    glVertex2f(-0.74f,-0.34f);
    glVertex2f(-0.82f,-0.58f);
    glVertex2f(-0.70f,-0.78f);
    glVertex2f(-0.82f,-0.90f);
    glVertex2f(-0.98f,-0.64f);
    glVertex2f(-1.10f,-0.34f);
    glEnd();
    glColor3f(0.13f*shade,0.61f*shade,0.18f*shade);
    glBegin(GL_POLYGON); // Eurasia
    glVertex2f(-0.30f,0.62f);
    glVertex2f(0.08f,0.78f);
    glVertex2f(0.55f,0.72f);
    glVertex2f(1.10f,0.68f);
    glVertex2f(1.70f,0.58f);
    glVertex2f(1.84f,0.30f);
    glVertex2f(1.50f,0.12f);
    glVertex2f(1.18f,0.02f);
    glVertex2f(1.02f,-0.16f);
    glVertex2f(0.72f,-0.02f);
    glVertex2f(0.48f,0.12f);
    glVertex2f(0.16f,0.20f);
    glVertex2f(-0.08f,0.34f);
    glEnd();
    glColor3f(0.17f*shade,0.56f*shade,0.15f*shade);
    glBegin(GL_POLYGON); // Africa
    glVertex2f(0.02f,0.26f);
    glVertex2f(0.34f,0.20f);
    glVertex2f(0.50f,-0.10f);
    glVertex2f(0.42f,-0.48f);
    glVertex2f(0.28f,-0.78f);
    glVertex2f(0.02f,-0.62f);
    glVertex2f(-0.12f,-0.20f);
    glVertex2f(-0.16f,0.10f);
    glEnd();
    glColor3f(0.12f*shade,0.56f*shade,0.15f*shade);
    glBegin(GL_POLYGON); // Australia
    glVertex2f(1.20f,-0.48f);
    glVertex2f(1.55f,-0.46f);
    glVertex2f(1.72f,-0.66f);
    glVertex2f(1.45f,-0.82f);
    glVertex2f(1.12f,-0.68f);
    glEnd();
    glColor3f(0.72f*shade,0.48f*shade,0.33f*shade);
    glBegin(GL_POLYGON); // Antarctica
    glVertex2f(-1.95f,-0.92f);
    glVertex2f(-1.15f,-0.86f);
    glVertex2f(-0.35f,-0.94f);
    glVertex2f(0.55f,-0.88f);
    glVertex2f(1.40f,-0.96f);
    glVertex2f(1.95f,-0.90f);
    glVertex2f(1.95f,-1.10f);
    glVertex2f(-1.95f,-1.10f);
    glEnd();
    glColor4f(0.90f,0.63f,0.27f,0.42f);
    drawEllipse(0.18f,0.02f,0.24f,0.10f,60);
    drawEllipse(0.70f,0.39f,0.28f,0.08f,60);
    drawEllipse(1.45f,-0.60f,0.16f,0.06f,40);
    glColor4f(0.40f,0.24f,0.14f,0.48f);
    drawEllipse(-1.54f,0.38f,0.040f,0.24f,50);
    drawEllipse(-0.92f,-0.42f,0.035f,0.34f,50);
    drawEllipse(0.86f,0.20f,0.22f,0.032f,50);
    drawEllipse(0.20f,-0.42f,0.052f,0.20f,50);
}

void drawOceanRelief() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.60f,0.82f,1.0f,0.16f);
    glLineWidth(1);
    for(int row=0;row<12;row++) {
        float y=-0.86f+row*0.15f;
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<=120;i++) {
            float x=-1.95f+i*(3.90f/120.0f);
            float wave=0.018f*sin(i*0.28f+row*0.70f);
            glVertex2f(x,y+wave);
        }
        glEnd();
    }
    glColor4f(0.02f,0.10f,0.30f,0.24f);
    drawEllipse(-1.45f,-0.22f,0.38f,0.20f,80);
    drawEllipse(-0.18f,-0.52f,0.42f,0.20f,80);
    drawEllipse(1.22f,-0.12f,0.46f,0.24f,80);
}

void drawMapContent(bool zoomed) {
    drawOceanRelief();
    drawMapLandShape(1.0f);
}

void drawMapScene(bool zoomed) {
    glClearColor(0.03f,0.17f,0.36f,1.0f);
    glColor3ub(28,82,139);
    drawRect(-2.0f,-1.2f,2.0f,1.2f);
    glPushMatrix();
    if(zoomed) {
        glScalef(mapScale,mapScale,1);
        glTranslatef(-0.78f,-0.02f,0);
    } else {
        glScalef(mapScale,mapScale,1);
    }
    drawMapContent(zoomed);
    glPopMatrix();
}

void display() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    if(sceneMode==1) drawMapScene(false);
    else if(sceneMode==2) drawMapScene(true);
    else {
        drawStarsF();
        drawSun();
        drawLensFlare();
        drawEarthMoonSystem();
        glColor3ub(220,225,245);
        drawText(-1.15f,-0.92f,"PAGE 3: W=wide map  E=scaled map  R=Earth-Moon  A=resume  S=stop  +/-=speed  Q=reset map");
    }
}

void keyboard(unsigned char key, int,int) {
    switch(key) {
        case 'w':
        case 'W':
            sceneMode=1;
            mapScale=1.0f;
            break;
        case 'e':
        case 'E':
            sceneMode=2;
            mapScale=2.20f;
            break;
        case 'r':
        case 'R':
            sceneMode=3;
            break;
        case 'a':
        case 'A':
            running=true;
            break;
        case 's':
        case 'S':
            running=false;
            break;
        case '+':
        case '=':
            if(sceneMode==1||sceneMode==2) {
                mapScale+=0.15f;
                if(mapScale>4) mapScale=4;
            } else {
                speedValue+=0.2f;
                if(speedValue>5) speedValue=5;
            }
            break;
        case '-':
            if(sceneMode==1||sceneMode==2) {
                mapScale-=0.15f;
                if(mapScale<0.6f) mapScale=0.6f;
            } else {
                speedValue-=0.2f;
                if(speedValue<0.2f) speedValue=0.2f;
            }
            break;
        case 'q':
        case 'Q':
            if(sceneMode==1) mapScale=1.0f;
            if(sceneMode==2) mapScale=2.20f;
            break;
    }
}

void update() {
    if(running) {
        earthSpin += 1.2f * speedValue;
        moonOrbit += 0.8f * speedValue;
        if(earthSpin>360) earthSpin-=360;
        if(moonOrbit>360) moonOrbit-=360;
    }
}
}

namespace DayNightScene {
float angleAll  = 0.0f;
float timeOfDay = 0.0f;
bool  isPaused  = false;

struct Star {
    float x, y;
    float brightness;
    float twinkleSpeed;
};
vector<Star> stars;

struct Bird {
    float x, y;
    float speed;
    float size;
    float wingAngle;
    float wingSpeed;
};
vector<Bird> birds;

struct Car {
    float x, y;
    float speed;
    float scale;
};
vector<Car> cars;

void drawCircle(float x, float y, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= seg; i++) {
        float theta = 2.0f * PI * i / seg;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

void getSkyColor(float t, float& r, float& g, float& b) {
    if (t < 0.25f) {
        float lt = t / 0.25f;
        r = 0.95f + (0.4f  - 0.95f) * lt;
        g = 0.5f  + (0.7f  - 0.5f)  * lt;
        b = 0.3f  + (1.0f  - 0.3f)  * lt;
    } else if (t < 0.5f) {
        float lt = (t - 0.25f) / 0.25f;
        r = 0.4f  + (1.0f  - 0.4f)  * lt;
        g = 0.7f  + (0.4f  - 0.7f)  * lt;
        b = 1.0f  + (0.1f  - 1.0f)  * lt;
    } else if (t < 0.75f) {
        float lt = (t - 0.5f) / 0.25f;
        r = 1.0f  + (0.0f  - 1.0f)  * lt;
        g = 0.4f  + (0.0f  - 0.4f)  * lt;
        b = 0.1f  + (0.05f - 0.1f)  * lt;
    } else {
        float lt = (t - 0.75f) / 0.25f;
        r = 0.0f  + (0.95f - 0.0f)  * lt;
        g = 0.0f  + (0.5f  - 0.0f)  * lt;
        b = 0.05f + (0.3f  - 0.05f) * lt;
    }
}

void drawSky() {
    float r, g, b;
    getSkyColor(timeOfDay, r, g, b);
    glBegin(GL_QUADS);
    glColor3f(r * 0.6f, g * 0.6f, b * 0.8f);
    glVertex2f(-1.0f,  1.0f);
    glVertex2f( 1.0f,  1.0f);
    glColor3f(r, g, b);
    glVertex2f( 1.0f, -0.3f);
    glVertex2f(-1.0f, -0.3f);
    glEnd();
}

void drawSun() {
    float visibility = 0.0f;
    if (timeOfDay < 0.5f)
        visibility = sin(timeOfDay / 0.5f * PI);
    if (visibility <= 0.0f) return;

    float sunAngle = PI + timeOfDay / 0.5f * PI;
    float sunX = 0.6f * cos(sunAngle);
    float sunY = 0.3f + 0.5f * sin(sunAngle);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 8; i++) {
        float ratio = (float)i / 8;
        glColor4f(1.0f, 0.9f, 0.5f, (1.0f - ratio) * 0.15f * visibility);
        drawCircle(sunX, sunY, 0.05f + ratio * 0.1f, 30);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1.0f, 0.95f * visibility, 0.4f * visibility);
    drawCircle(sunX, sunY, 0.07f, 40);
}

void drawStars() {
    float visibility = 0.0f;
    if (timeOfDay > 0.6f && timeOfDay < 1.0f) {
        visibility = (timeOfDay - 0.6f) / 0.15f;
        if (visibility > 1.0f) visibility = 1.0f;
    } else if (timeOfDay < 0.15f) {
        visibility = 1.0f - timeOfDay / 0.15f;
    }
    if (visibility <= 0.0f) return;

    glEnable(GL_BLEND);
    for (auto& s : stars) {
        float twinkle = 0.6f + 0.4f * sin(angleAll * s.twinkleSpeed * 1.5f + s.x * 15);
        float bright  = s.brightness * twinkle * visibility;

        float colorPhase = fmod(s.x * 50 + s.y * 30, 4.0f);
        float r, g, b;
        if      (colorPhase < 1.0f) { r=0.7f; g=0.8f; b=1.0f; }
        else if (colorPhase < 2.0f) { r=1.0f; g=1.0f; b=1.0f; }
        else if (colorPhase < 3.0f) { r=1.0f; g=0.95f;b=0.7f; }
        else                        { r=1.0f; g=0.8f; b=0.6f; }

        glColor4f(r * bright, g * bright, b * bright, 1.0f);
        glPointSize(1.5f + bright * 2.0f);
        glBegin(GL_POINTS);
        glVertex2f(s.x, s.y);
        glEnd();
    }
}

void drawMoon() {
    float visibility = 0.0f;
    if (timeOfDay > 0.65f) {
        visibility = (timeOfDay - 0.65f) / 0.1f;
        if (visibility > 1.0f) visibility = 1.0f;
    } else if (timeOfDay < 0.1f) {
        visibility = 1.0f - timeOfDay / 0.1f;
    }
    if (visibility <= 0.0f) return;

    float moonX = 0.4f, moonY = 0.55f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 8; i++) {
        float ratio = (float)i / 8;
        glColor4f(0.9f, 0.92f, 1.0f, (1.0f - ratio) * 0.08f * visibility);
        drawCircle(moonX, moonY, 0.06f + ratio * 0.05f, 25);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.95f, 0.95f, 0.98f, visibility);
    drawCircle(moonX, moonY, 0.05f, 35);

    float sr, sg, sb;
    getSkyColor(timeOfDay, sr, sg, sb);
    glColor3f(sr * 0.6f, sg * 0.6f, sb * 0.8f);
    drawCircle(moonX + 0.025f, moonY, 0.045f, 35);
}

void drawBird(float x, float y, float size, float wingAngle) {
    glLineWidth(1.5f);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    float leftDeg = sin(wingAngle) * 40.0f;
    glRotatef(leftDeg, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f,  0.0f);
    glVertex2f(-size, 0.0f);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(-leftDeg, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(size, 0.0f);
    glEnd();
    glPopMatrix();
}

void drawBirds() {
    float visibility = 0.0f;
    if (timeOfDay > 0.05f && timeOfDay < 0.55f) {
        if      (timeOfDay < 0.15f) visibility = (timeOfDay - 0.05f) / 0.1f;
        else if (timeOfDay > 0.45f) visibility = 1.0f - (timeOfDay - 0.45f) / 0.1f;
        else                        visibility = 1.0f;
    }
    if (visibility <= 0.0f) return;

    glEnable(GL_BLEND);
    glColor4f(0.05f, 0.05f, 0.05f, visibility);
    for (auto& bird : birds)
        drawBird(bird.x, bird.y, bird.size, bird.wingAngle);
}

void drawCar(float x, float y, float sc, bool goingRight) {
    float t = timeOfDay;
    float dir = goingRight ? 1.0f : -1.0f;

    glColor3f(0.8f, 0.15f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.12f*sc,       y);
    glVertex2f(x + 0.12f*sc,       y);
    glVertex2f(x + 0.12f*sc,       y + 0.05f*sc);
    glVertex2f(x - 0.12f*sc,       y + 0.05f*sc);
    glEnd();

    glColor3f(0.75f, 0.12f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.07f*sc,       y + 0.05f*sc);
    glVertex2f(x + 0.07f*sc,       y + 0.05f*sc);
    glVertex2f(x + 0.06f*sc,       y + 0.09f*sc);
    glVertex2f(x - 0.06f*sc,       y + 0.09f*sc);
    glEnd();

    if (t > 0.6f || t < 0.1f)
        glColor3f(1.0f, 0.95f, 0.5f);
    else
        glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.05f*sc, y + 0.055f*sc);
    glVertex2f(x + 0.05f*sc, y + 0.055f*sc);
    glVertex2f(x + 0.05f*sc, y + 0.082f*sc);
    glVertex2f(x - 0.05f*sc, y + 0.082f*sc);
    glEnd();

    float wheelRot = (x / (0.025f * sc)) * (180.0f / PI);

    for (int w = 0; w < 2; w++) {
        float wx = (w == 0) ? x - 0.07f*sc : x + 0.07f*sc;
        float wy = y;
        float wr = 0.025f * sc;

        glColor3f(0.1f, 0.1f, 0.1f);
        drawCircle(wx, wy, wr, 16);

        glColor3f(0.5f, 0.5f, 0.5f);
        drawCircle(wx, wy, wr * 0.4f, 10);

        glColor3f(0.6f, 0.6f, 0.6f);
        glLineWidth(1.2f);
        glPushMatrix();
        glTranslatef(wx, wy, 0.0f);
        glRotatef(wheelRot, 0.0f, 0.0f, 1.0f);
        for (int s = 0; s < 4; s++) {
            float a = s * 45.0f * PI / 180.0f;
            glBegin(GL_LINES);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(wr * 0.85f * cos(a), wr * 0.85f * sin(a));
            glEnd();
        }
        glPopMatrix();
    }

    if (t > 0.6f || t < 0.15f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float headX = x + dir * 0.12f*sc;
        glColor4f(1.0f, 1.0f, 0.6f, 0.4f);
        drawCircle(headX, y + 0.025f*sc, 0.03f*sc, 12);
        glColor4f(1.0f, 1.0f, 0.5f, 0.15f);
        drawCircle(headX, y + 0.025f*sc, 0.07f*sc, 12);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void drawCars() {
    for (auto& car : cars) {
        bool goingRight = car.speed > 0;
        drawCar(car.x, car.y, car.scale, goingRight);
    }
}

void drawHouse(float x, float y, float scale) {
    float t = timeOfDay;
    float wallR, wallG, wallB;
    if (t < 0.5f) { wallR=0.85f; wallG=0.7f;  wallB=0.55f; }
    else           { wallR=0.3f;  wallG=0.25f; wallB=0.2f;  }

    glColor3f(wallR, wallG, wallB);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.12f*scale, y);
    glVertex2f(x + 0.12f*scale, y);
    glVertex2f(x + 0.12f*scale, y + 0.15f*scale);
    glVertex2f(x - 0.12f*scale, y + 0.15f*scale);
    glEnd();

    glColor3f(0.6f, 0.2f, 0.15f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.15f*scale, y + 0.15f*scale);
    glVertex2f(x + 0.15f*scale, y + 0.15f*scale);
    glVertex2f(x,               y + 0.28f*scale);
    glEnd();

    glColor3f(0.4f, 0.25f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.03f*scale, y);
    glVertex2f(x + 0.03f*scale, y);
    glVertex2f(x + 0.03f*scale, y + 0.07f*scale);
    glVertex2f(x - 0.03f*scale, y + 0.07f*scale);
    glEnd();

    if (t > 0.6f || t < 0.1f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.9f, 0.3f, 0.3f);
        drawCircle(x - 0.06f*scale, y + 0.1f*scale, 0.04f*scale, 15);
        drawCircle(x + 0.06f*scale, y + 0.1f*scale, 0.04f*scale, 15);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1.0f, 0.95f, 0.5f);
    } else {
        glColor3f(0.6f, 0.85f, 1.0f);
    }

    glBegin(GL_QUADS);
    glVertex2f(x - 0.085f*scale, y + 0.08f*scale);
    glVertex2f(x - 0.035f*scale, y + 0.08f*scale);
    glVertex2f(x - 0.035f*scale, y + 0.12f*scale);
    glVertex2f(x - 0.085f*scale, y + 0.12f*scale);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x + 0.035f*scale, y + 0.08f*scale);
    glVertex2f(x + 0.085f*scale, y + 0.08f*scale);
    glVertex2f(x + 0.085f*scale, y + 0.12f*scale);
    glVertex2f(x + 0.035f*scale, y + 0.12f*scale);
    glEnd();

    glColor3f(0.4f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(x + 0.06f*scale, y + 0.22f*scale);
    glVertex2f(x + 0.09f*scale, y + 0.22f*scale);
    glVertex2f(x + 0.09f*scale, y + 0.32f*scale);
    glVertex2f(x + 0.06f*scale, y + 0.32f*scale);
    glEnd();
}

void drawRoad() {
    float t = timeOfDay;

    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.33f);
    glVertex2f( 1.0f, -0.33f);
    glVertex2f( 1.0f, -0.27f);
    glVertex2f(-1.0f, -0.27f);
    glEnd();

    if (t > 0.6f || t < 0.15f)
        glColor3f(0.9f, 0.8f, 0.2f);
    else
        glColor3f(0.85f, 0.85f, 0.2f);

    for (int i = 0; i < 8; i++) {
        float dx = -0.9f + i * 0.27f;
        glBegin(GL_QUADS);
        glVertex2f(dx,          -0.302f);
        glVertex2f(dx + 0.12f,  -0.302f);
        glVertex2f(dx + 0.12f,  -0.298f);
        glVertex2f(dx,          -0.298f);
        glEnd();
    }
}


void drawGround() {
    float t = timeOfDay;
    float gr, gg, gb;
    if      (t < 0.25f) { float lt=t/0.25f;           gr=0.1f+0.1f*lt; gg=0.3f+0.2f*lt; gb=0.05f; }
    else if (t < 0.5f)  { float lt=(t-0.25f)/0.25f;   gr=0.2f; gg=0.5f-0.2f*lt; gb=0.05f; }
    else if (t < 0.75f) { float lt=(t-0.5f)/0.25f;    gr=0.2f-0.18f*lt; gg=0.3f-0.28f*lt; gb=0.05f-0.04f*lt; }
    else                { float lt=(t-0.75f)/0.25f;    gr=0.02f+0.08f*lt; gg=0.02f+0.28f*lt; gb=0.01f+0.04f*lt; }

    glBegin(GL_QUADS);
    glColor3f(gr, gg, gb);
    glVertex2f(-1.0f, -0.3f);
    glVertex2f( 1.0f, -0.3f);
    glColor3f(gr*0.5f, gg*0.5f, gb*0.5f);
    glVertex2f( 1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
}


void drawStreetLights() {
    float px     =  0.55f;
    float groundY = -0.30f;

    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(px, groundY);
    glVertex2f(px, groundY + 0.20f);
    glEnd();

    if (timeOfDay > 0.6f || timeOfDay < 0.15f)
        glColor3f(1.0f, 1.0f, 0.4f);
    else
        glColor3f(0.6f, 0.6f, 0.6f);

    drawCircle(px, groundY + 0.20f, 0.015f, 12);
    glLineWidth(1.0f);
}

void drawPauseText() {
    if (!isPaused) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.45f);
    glBegin(GL_QUADS);
    glVertex2f(-0.38f,  0.13f);
    glVertex2f( 0.38f,  0.13f);
    glVertex2f( 0.38f, -0.05f);
    glVertex2f(-0.38f, -0.05f);
    glEnd();
    glColor3f(1.0f, 1.0f, 0.3f);
    glRasterPos2f(-0.30f, 0.02f);
    const char* msg = "|| PAUSED  [SPACE / P] to Resume";
    for (int i = 0; msg[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();
    drawSun();
    drawStars();
    drawMoon();
    drawBirds();
    drawGround();
    drawRoad();
    drawHouse(0.55f, -0.28f, 1.0f);
    drawHouse(0.78f, -0.25f, 0.75f);
    drawCars();
    drawStreetLights();
    drawPauseText();

}


void update() {
    if (!isPaused) {
        angleAll  += 0.5f;
        timeOfDay += 0.0005f;
        if (timeOfDay > 1.0f) timeOfDay = 0.0f;

        for (auto& bird : birds) {
            bird.x += bird.speed;
            bird.wingAngle += bird.wingSpeed;
            if (bird.x > 1.5f) bird.x = -1.5f;
        }

        for (auto& car : cars) {
            car.x += car.speed;
            if (car.speed > 0 && car.x >  1.5f) car.x = -1.5f;
            if (car.speed < 0 && car.x < -1.5f) car.x =  1.5f;
        }
    }

}

void keyboard(unsigned char key) {
    if (key == 27) exit(0);
    if (key == ' ' || key == 'p' || key == 'P')
        isPaused = !isPaused;
    if (!isPaused) {
        if (key == 'f' || key == 'F') timeOfDay += 0.05f;
        if (key == 'b' || key == 'B') timeOfDay -= 0.05f;
        if (timeOfDay > 1.0f) timeOfDay = 0.0f;
        if (timeOfDay < 0.0f) timeOfDay = 1.0f;
    }
    glutPostRedisplay();
}

void init() {
    srand((unsigned)time(0));
    stars.clear();
    birds.clear();
    cars.clear();

for (int i = 0; i < 300; i++) {
    Star s;
    s.x           = -1.2f + (rand() % 240) / 100.0f;
    s.y           =  0.0f + (rand() % 160) / 100.0f;
    s.brightness  =  0.3f + (rand() % 70)  / 100.0f;
    s.twinkleSpeed =  0.5f + (rand() % 15)  / 10.0f;
    stars.push_back(s);
}

for (int i = 0; i < 6; i++) {
    Bird bird;
    bird.x          = -1.5f + i * 0.4f;
    bird.y          =  0.3f + (i % 3) * 0.1f;
    bird.speed      =  0.003f + i * 0.001f;
    bird.size       =  0.04f  + (i % 2) * 0.02f;
    bird.wingAngle  =  i * 1.0f;
    bird.wingSpeed  =  0.1f   + i * 0.02f;
    birds.push_back(bird);
}

Car c1; c1.x = -1.5f; c1.y = -0.31f; c1.speed =  0.004f; c1.scale = 1.0f;
Car c2; c2.x =  1.5f; c2.y = -0.31f; c2.speed = -0.005f; c2.scale = 0.9f;
cars.push_back(c1);
cars.push_back(c2);


glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glEnable(GL_POINT_SMOOTH);
glEnable(GL_LINE_SMOOTH);

}
}

namespace VillageScene {
float boatX=0;
float cloud1X=0;
float cloud2X=0.3f;
float cloud3X=-0.5f;
float cloud4X=0.7f;
float birdAngle=0;
float smokeOffset=0;
float dayNightCycle=0;
int isNight=0;
int frame=0;

struct StarV { float x,y,size,twinkleSpeed,phase; };
StarV starsV[60];
int numStarsV=60;

void initStarsV() {
    srand((unsigned)time(nullptr));
    for(int i=0;i<numStarsV;i++) {
        starsV[i].x=-0.98f+(rand()%196)/100.0f;
        starsV[i].y=0.1f+(rand()%90)/100.0f;
        starsV[i].size=0.002f+(rand()%5)/1000.0f;
        starsV[i].twinkleSpeed=0.01f+(rand()%10)/100.0f;
        starsV[i].phase=(rand()%100)/10.0f;
    }
}

void drawStarsV() {
    if(dayNightCycle<0.2f) return;
    float alpha=(dayNightCycle-0.2f)/0.8f;
    if(alpha>1) alpha=1;
    for(int i=0;i<numStarsV;i++) {
        float twinkle=0.4f+0.6f*sin(frame*starsV[i].twinkleSpeed+starsV[i].phase);
        float starAlpha=alpha*twinkle;
        glColor4f(1,1,1,starAlpha);
        drawCircle(starsV[i].x,starsV[i].y,starsV[i].size,20);
        if(starsV[i].size>0.004f && twinkle>0.7f) {
            glColor4f(1,0.9f,0.5f,starAlpha*0.6f);
            drawCircle(starsV[i].x,starsV[i].y,starsV[i].size*1.8f,20);
        }
    }
}

void drawBirdV(float x,float y,float size,float wingAngle) {
    if(dayNightCycle>0.7f) return;
    float alpha=1.0f-(dayNightCycle-0.7f)/0.3f;
    if(alpha<0) alpha=0;
    glColor4f(0.1f,0.1f,0.1f,alpha);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x-0.02f,y);
    glVertex2f(x+0.02f,y);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x,y);
    glVertex2f(x-0.04f*size*cos(wingAngle), y+0.03f*size*sin(wingAngle));
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(x,y);
    glVertex2f(x+0.04f*size*cos(wingAngle), y+0.03f*size*sin(wingAngle));
    glEnd();
}

void drawTreeV(float x,float y) {
    glColor3f(0.4f-dayNightCycle*0.25f, 0.2f-dayNightCycle*0.1f, 0);
    drawRect(x-0.02f,y,x+0.02f,y+0.18f);
    glColor3f(0,0.5f-dayNightCycle*0.35f,0);
    drawCircle(x,y+0.25f,0.08f,40);
    drawCircle(x-0.06f,y+0.2f,0.07f,40);
    drawCircle(x+0.06f,y+0.2f,0.07f,40);
}

void drawCloudV(float x,float y) {
    float bright=0.98f-dayNightCycle*0.68f;
    glColor3f(bright,bright,bright);
    drawCircle(x,y,0.06f,30);
    drawCircle(x+0.05f,y+0.02f,0.07f,30);
    drawCircle(x-0.05f,y+0.01f,0.05f,30);
    drawCircle(x+0.03f,y-0.02f,0.05f,30);
}

void drawHouseV(float x,float y) {
    glColor4f(0,0,0,0.2f);
    drawRect(x-0.25f,y-0.03f,x+0.2f,y+0.02f);
    glColor3f(0.85f-dayNightCycle*0.45f,0.75f-dayNightCycle*0.4f,0.65f-dayNightCycle*0.35f);
    drawRect(x-0.15f,y,x+0.15f,y+0.22f);
    glColor3f(0.7f-dayNightCycle*0.4f,0.6f-dayNightCycle*0.35f,0.5f-dayNightCycle*0.3f);
    drawRect(x-0.22f,y,x-0.15f,y+0.22f);
    glColor3f(0.6f-dayNightCycle*0.3f,0.2f-dayNightCycle*0.1f,0.1f-dayNightCycle*0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(x-0.24f,y+0.22f);
    glVertex2f(x+0.17f,y+0.22f);
    glVertex2f(x+0.04f,y+0.40f);
    glVertex2f(x-0.11f,y+0.40f);
    glEnd();
    glColor3f(0.5f-dayNightCycle*0.25f,0.25f-dayNightCycle*0.12f,0.1f-dayNightCycle*0.05f);
    drawRect(x+0.07f,y+0.22f,x+0.12f,y+0.35f);
    for(int i=0;i<3;i++) {
        float off=smokeOffset+i*0.4f;
        float sx=x+0.095f+sin(off*2)*0.005f;
        float sy=y+0.36f+off*0.06f;
        glColor4f(0.6f,0.6f,0.7f,0.3f-i*0.1f);
        drawCircle(sx,sy,0.01f+i*0.005f,20);
    }
    glColor3f(0.4f-dayNightCycle*0.2f,0.15f-dayNightCycle*0.07f,0.05f-dayNightCycle*0.03f);
    drawRect(x-0.02f,y,x+0.06f,y+0.12f);
    drawCircle(x+0.02f,y+0.12f,0.045f,30);
    glColor3f(0.9f,0.7f,0.2f);
    drawCircle(x+0.045f,y+0.06f,0.005f,10);
    float winR=0.7f+dayNightCycle*0.25f;
    float winG=0.9f-dayNightCycle*0.05f;
    float winB=1.0f-dayNightCycle*0.5f;
    glColor3f(winR,winG,winB);
    drawRect(x-0.12f,y+0.07f,x-0.05f,y+0.16f);
    drawRect(x+0.08f,y+0.07f,x+0.14f,y+0.16f);
    glColor3f(0.3f,0.15f,0.05f);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x-0.085f,y+0.07f);
    glVertex2f(x-0.085f,y+0.16f);
    glVertex2f(x-0.12f,y+0.115f);
    glVertex2f(x-0.05f,y+0.115f);
    glVertex2f(x+0.11f,y+0.07f);
    glVertex2f(x+0.11f,y+0.16f);
    glVertex2f(x+0.08f,y+0.115f);
    glVertex2f(x+0.14f,y+0.115f);
    glEnd();
}

void drawBoatV() {
    glColor3f(0.5f-dayNightCycle*0.25f,0.25f-dayNightCycle*0.13f,0.1f-dayNightCycle*0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.12f+boatX,-0.76f);
    glVertex2f(0.12f+boatX,-0.76f);
    glVertex2f(0.07f+boatX,-0.86f);
    glVertex2f(-0.07f+boatX,-0.86f);
    glEnd();
    glColor3f(0.6f-dayNightCycle*0.3f,0.35f-dayNightCycle*0.2f,0.15f-dayNightCycle*0.08f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.10f+boatX,-0.76f);
    glVertex2f(0.10f+boatX,-0.76f);
    glVertex2f(0.06f+boatX,-0.80f);
    glVertex2f(-0.06f+boatX,-0.80f);
    glEnd();
    glColor3f(0.3f-dayNightCycle*0.15f,0.15f-dayNightCycle*0.07f,0.05f-dayNightCycle*0.02f);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0+boatX,-0.76f);
    glVertex2f(0+boatX,-0.56f);
    glEnd();
    float sailBright=1.0f-dayNightCycle*0.5f;
    glColor3f(sailBright,sailBright,sailBright);
    glBegin(GL_TRIANGLES);
    glVertex2f(0+boatX,-0.58f);
    glVertex2f(0+boatX,-0.74f);
    glVertex2f(0.10f+boatX,-0.66f);
    glEnd();
    glColor3f(1.0f-dayNightCycle*0.4f,0.2f,0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0+boatX,-0.56f);
    glVertex2f(0.06f+boatX,-0.58f);
    glVertex2f(0+boatX,-0.60f);
    glEnd();
}

void drawMoonV() {
    if(dayNightCycle<0.4f) return;
    float alpha=(dayNightCycle-0.4f)/0.6f;
    glColor4f(0.95f,0.95f,0.9f,alpha);
    drawCircle(0.7f,0.78f,0.08f,50);
    glColor4f(0.85f,0.85f,0.8f,alpha*0.5f);
    drawCircle(0.68f,0.80f,0.015f,20);
    drawCircle(0.72f,0.76f,0.012f,20);
    drawCircle(0.70f,0.82f,0.008f,20);
}

void drawSkyV() {
    float topR=0.0f+dayNightCycle*0.02f;
    float topG=0.1f+dayNightCycle*0.02f;
    float topB=0.5f-dayNightCycle*0.4f;
    float botR=0.4f-dayNightCycle*0.35f;
    float botG=0.8f-dayNightCycle*0.75f;
    float botB=1.0f-dayNightCycle*0.8f;
    glBegin(GL_POLYGON);
    glColor3f(topR,topG,topB);
    glVertex2f(-1,1);
    glVertex2f(1,1);
    glColor3f(botR,botG,botB);
    glVertex2f(1,0);
    glVertex2f(-1,0);
    glEnd();
}

void drawSunV() {
    if(dayNightCycle>0.6f) return;
    float alpha=(dayNightCycle>0.4f)?1.0f-(dayNightCycle-0.4f)/0.2f:1.0f;
    glColor4f(1,1,0.3f,alpha*0.2f);
    drawCircle(-0.75f,0.8f,0.13f,50);
    glColor4f(1,0.95f,0.1f,alpha);
    drawCircle(-0.75f,0.8f,0.09f,50);
}

void drawHillsV() {
    glColor3f(0.35f-dayNightCycle*0.25f,0.65f-dayNightCycle*0.45f,0.25f-dayNightCycle*0.15f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.2f,-0.45f);
    glVertex2f(-0.5f,0.35f);
    glVertex2f(0.2f,-0.45f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.1f,-0.45f);
    glVertex2f(0.5f,0.4f);
    glVertex2f(1.1f,-0.45f);
    glEnd();
    glColor3f(0.25f-dayNightCycle*0.17f,0.55f-dayNightCycle*0.35f,0.2f-dayNightCycle*0.12f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.6f,-0.45f);
    glVertex2f(-0.15f,0.25f);
    glVertex2f(0.3f,-0.45f);
    glEnd();
}

void drawGroundV() {
    glColor3f(0.2f-dayNightCycle*0.15f,0.7f-dayNightCycle*0.5f,0.1f-dayNightCycle*0.05f);
    drawRect(-1,-0.45f,1,0);
    glColor3f(0.25f-dayNightCycle*0.17f,0.75f-dayNightCycle*0.55f,0.15f-dayNightCycle*0.1f);
    drawRect(-1,-0.45f,1,-0.35f);
}

void drawRiverV() {
    glColor3f(0.3f-dayNightCycle*0.25f,0.75f-dayNightCycle*0.6f,0.95f-dayNightCycle*0.7f);
    drawRect(-1,-0.45f,1,-1);
    float waveAlpha=0.5f-dayNightCycle*0.35f;
    glColor4f(0.5f,0.9f,1,waveAlpha);
    glLineWidth(1.5);
    for(int row=0;row<3;row++) {
        float y=-0.55f-row*0.1f;
        glBegin(GL_LINE_STRIP);
        for(float x=-0.98f;x<=0.98f;x+=0.03f) {
            float wave=sin(x*8.0f+frame*0.03f+row)*0.015f;
            glVertex2f(x,y+wave);
        }
        glEnd();
    }
}

void drawRoadV() {
    glColor3f(0.75f-dayNightCycle*0.5f,0.65f-dayNightCycle*0.45f,0.4f-dayNightCycle*0.25f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.45f,-0.02f);
    glVertex2f(-0.30f,0.06f);
    glVertex2f(0.25f,-0.45f);
    glVertex2f(0.05f,-0.45f);
    glEnd();
    glColor3f(0.55f-dayNightCycle*0.3f,0.45f-dayNightCycle*0.25f,0.25f-dayNightCycle*0.15f);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(-0.45f,-0.02f);
    glVertex2f(0.05f,-0.45f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(-0.30f,0.06f);
    glVertex2f(0.25f,-0.45f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawSkyV();
    drawStarsV();
    drawMoonV();
    drawSunV();
    drawHillsV();
    drawGroundV();
    drawRoadV();
    drawCloudV(-0.7f+cloud1X,0.78f);
    drawCloudV(-0.1f+cloud2X,0.72f);
    drawCloudV(0.5f+cloud3X,0.75f);
    drawCloudV(0.9f+cloud4X,0.80f);
    float birdCX=-0.3f,birdCY=0.55f,birdR=0.2f;
    float bx1=birdCX+birdR*cos(birdAngle);
    float by1=birdCY+birdR*sin(birdAngle);
    drawBirdV(bx1,by1,1.0f,sin(birdAngle*3)*0.5f+0.5f);
    float bx2=birdCX+birdR*cos(birdAngle+PI);
    float by2=birdCY+birdR*sin(birdAngle+PI);
    drawBirdV(bx2,by2,0.8f,sin((birdAngle+PI)*3)*0.5f+0.5f);
    float bx3=birdCX+birdR*cos(birdAngle+PI/2);
    float by3=birdCY+birdR*sin(birdAngle+PI/2);
    drawBirdV(bx3,by3,0.9f,sin((birdAngle+PI/2)*3)*0.5f+0.5f);
    drawTreeV(-0.7f,-0.15f);
    drawTreeV(-0.4f,-0.17f);
    drawTreeV(0.85f,-0.13f);
    drawTreeV(-0.85f,-0.2f);
    drawTreeV(0.1f,-0.18f);
    drawHouseV(0.5f,-0.28f);
    drawRiverV();
    drawBoatV();
}

void update() {
    frame++;
    smokeOffset+=0.05f;
    if(isNight==0) {
        dayNightCycle+=0.0008f;
        if(dayNightCycle>=1.0f) {
            dayNightCycle=1.0f;
            isNight=1;
        }
    } else {
        dayNightCycle-=0.0008f;
        if(dayNightCycle<=0.0f) {
            dayNightCycle=0.0f;
            isNight=0;
        }
    }
    boatX+=0.004f;
    if(boatX>1.3f) boatX=-1.3f;
    cloud1X-=0.002f;
    if(cloud1X<-1.5f) cloud1X=1.5f;
    cloud2X-=0.0015f;
    if(cloud2X<-1.5f) cloud2X=1.5f;
    cloud3X-=0.0025f;
    if(cloud3X<-1.5f) cloud3X=1.5f;
    cloud4X-=0.0018f;
    if(cloud4X<-1.5f) cloud4X=1.5f;
    birdAngle+=0.018f;
    if(birdAngle>2*PI) birdAngle-=2*PI;
}

void keyboard(unsigned char key) {
    switch(key) {
        case 'd':
            isNight=0;
            dayNightCycle=0.0f;
            break;
        case 'n':
            isNight=1;
            dayNightCycle=1.0f;
            break;
        case 'r':
            boatX=0;
            cloud1X=0;
            cloud2X=0.3f;
            cloud3X=-0.5f;
            cloud4X=0.7f;
            birdAngle=0;
            break;
    }
}

void init() {
    initStarsV();
}

void display() {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    if(currentPage == 1 || currentPage == 2) {
        glLoadIdentity();
        if(currentPage==1) drawPage1SolarSystem();
        else drawPage2PlanetSelection();
    } else if(currentPage == 3) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        float aspect = (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT);
        if(aspect>=1) gluOrtho2D(-aspect, aspect, -1.0, 1.0);
        else gluOrtho2D(-1.0, 1.0, -1.0/aspect, 1.0/aspect);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        Ftask3::display();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else if(currentPage == 4) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1,1,-1,1,-1,1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        DayNightScene::display();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else if(currentPage == 5) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(-1,1,-1,1,-1,1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        VillageScene::display();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w/(float)h;
    if(w>=h) gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    else gluOrtho2D(-1.0, 1.0, -1.0/aspect, 1.0/aspect);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if(key==27) exit(0);
    if(key>='1' && key<='5') {
        currentPage = key-'0';
        if(currentPage==2) selectedPlanet=-1;
        if(currentPage==3) Ftask3::sceneMode=3;
        glutPostRedisplay();
        return;
    }
    if(currentPage==1) {
        switch(key) {
            case 'r':
            case 'R':
                rotateFlag=1;
                break;
            case 's':
            case 'S':
                rotateFlag=0;
                break;
            case '+':
            case '=':
                targetSpeed+=0.1f;
                if(targetSpeed>5) targetSpeed=5;
                break;
            case '-':
                targetSpeed-=0.1f;
                if(targetSpeed<0.05f) targetSpeed=0.05f;
                break;
            case 'z':
                worldScale+=0.1f;
                break;
            case 'x':
                worldScale-=0.1f;
                if(worldScale<0.5f) worldScale=0.5f;
                break;
            case 'q':
            case 'Q':
                worldScale=1.0f;
                break;
        }
    } else if(currentPage==2) {
        if((key>='a' && key<='h')) selectedPlanet=key-'a';
        else if((key>='A' && key<='H')) selectedPlanet=key-'A';
        else if(key=='z'||key=='Z') selectedPlanet=-1;
    } else if(currentPage==3) {
        Ftask3::keyboard(key,x,y);
    } else if(currentPage==4) {
        DayNightScene::keyboard(key);
    } else if(currentPage==5) {
        VillageScene::keyboard(key);
    }
    glutPostRedisplay();
}

void updateTimer(int value) {
    if(currentPage==1 || currentPage==2) {
        if(showMeteor) {
            meteorX+=0.02f;
            meteorY-=0.01f;
            if(meteorX>1.2f||meteorY<-1.2f) showMeteor=false;
        } else if(rand()%200==0) {
            meteorX=-1.0f;
            meteorY=0.8f;
            showMeteor=true;
        }
        moonAngle+=5.0f*speed;
        angleAll+=0.025f;
        speed += (targetSpeed-speed)*0.05f;
        if(rotateFlag) {
            mercuryAngle+=4.0f*speed;
            venusAngle+=3.0f*speed;
            earthAngle+=2.5f*speed;
            marsAngle+=2.0f*speed;
            jupiterAngle+=1.2f*speed;
            saturnAngle+=1.0f*speed;
            uranusAngle+=0.8f*speed;
            neptuneAngle+=0.6f*speed;
        }
    } else if(currentPage==3) {
        Ftask3::update();
    } else if(currentPage==4) {
        DayNightScene::update();
    } else if(currentPage==5) {
        VillageScene::update();
    }
    glutPostRedisplay();
    glutTimerFunc(16, updateTimer, 0);
}

int main(int argc, char** argv) {
    srand((unsigned)time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
    glutCreateWindow("Merged: Solar System | Gallery | Earth-Moon | Day-Night | Village");
    initStarsOrig();
    initAsteroids();
    Ftask3::initStarsF();
    DayNightScene::init();
    VillageScene::init();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, updateTimer, 0);
    glutMainLoop();
    return 0;
}
