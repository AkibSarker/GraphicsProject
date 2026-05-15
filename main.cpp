#define GL_SILENCE_DEPRECATION
#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

#define PI 3.141516

using namespace std;

const int SCR_WIDTH = 1000;
const int SCR_HEIGHT = 520;

float earthSpin = 0.0f;
float moonOrbit = 0.0f;
float speedValue = 1.0f;
int sceneMode = 3;
float mapScale = 1.0f;
bool running = true;

struct Star {
    float x, y, brightness;
};

vector<Star> stars;

void drawCircle(float x, float y, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= seg; i++) {
        float theta = 2.0f * PI * i / seg;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}

void drawBigText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawEllipse(float x, float y, float rx, float ry, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= seg; i++) {
        float theta = 2.0f * PI * i / seg;
        glVertex2f(x + rx * cos(theta), y + ry * sin(theta));
    }
    glEnd();
}

void drawGlow(float x, float y, float r, int red, int green, int blue) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (int i = 0; i < 18; i++) {
        float ratio = (float)i / 18.0f;
        float currentRadius = r * (1.0f + ratio * 2.5f);
        float alpha = (1.0f - ratio) * 0.28f;

        glColor4f(red / 255.0f, green / 255.0f, blue / 255.0f, alpha);
        drawCircle(x, y, currentRadius, 90);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initStars() {
    stars.clear();
    for (int i = 0; i < 350; i++) {
        Star s;
        s.x = (rand() % 240 - 120) / 100.0f;
        s.y = (rand() % 200 - 100) / 100.0f;
        s.brightness = 0.25f + (rand() % 75) / 100.0f;
        stars.push_back(s);
    }
}

void drawStars() {
    glPointSize(1.2f);
    glBegin(GL_POINTS);
    for (int i = 0; i < (int)stars.size(); i++) {
        float b = stars[i].brightness;
        glColor3f(b, b, b);
        glVertex2f(stars[i].x, stars[i].y);
    }
    glEnd();
}

void drawTopBar() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.02f, 0.11f, 0.18f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(-0.62f, 0.92f);
    glVertex2f( 0.62f, 0.92f);
    glVertex2f( 0.62f, 1.00f);
    glVertex2f(-0.62f, 1.00f);
    glEnd();

    for (int i = 0; i < 5; i++) {
        float x = -0.58f + i * 0.10f;
        if (i == 2) glColor3ub(20, 190, 225);
        else glColor4f(0.08f, 0.18f, 0.27f, 0.75f);

        glBegin(GL_QUADS);
        glVertex2f(x, 0.945f);
        glVertex2f(x + 0.05f, 0.945f);
        glVertex2f(x + 0.05f, 0.980f);
        glVertex2f(x, 0.980f);
        glEnd();
    }

    glColor3ub(0, 190, 130);
    glBegin(GL_QUADS);
    glVertex2f(0.35f, 0.945f);
    glVertex2f(0.41f, 0.945f);
    glVertex2f(0.41f, 0.980f);
    glVertex2f(0.35f, 0.980f);
    glEnd();
}

void drawSun() {
    glPushMatrix();
    glTranslatef(-0.48f, 0.62f, 0.0f);

    drawGlow(0, 0, 0.13f, 255, 210, 60);
    drawGlow(0, 0, 0.09f, 255, 255, 180);

    glColor3ub(255, 255, 210);
    drawCircle(0, 0, 0.125f, 100);

    glColor3ub(255, 230, 80);
    drawCircle(0, 0, 0.035f, 60);

    glPopMatrix();
}

void drawLensFlare() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float x = -0.05f;
    float y = 0.66f;
    for (int i = 0; i < 5; i++) {
        float r = 0.035f + i * 0.012f;
        glColor4f(0.45f, 0.05f, 0.45f, 0.08f);
        drawCircle(x + i * 0.08f, y - i * 0.025f, r, 50);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawOrbitLine(float radius) {
    glColor4f(0.7f, 0.8f, 1.0f, 0.18f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 220; i++) {
        float theta = 2.0f * PI * i / 220.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }
    glEnd();
}

float getDarkSideAngle(float bodyX, float bodyY) {
    float sunX = -0.48f;
    float sunY = 0.62f;

    float darkX = bodyX - sunX;
    float darkY = bodyY - sunY;

    return atan2(darkY, darkX) * 180.0f / PI;
}

void drawDirectionalShadow(float radius, float darkAngleDeg, float alpha) {
    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);

    for (int i = -90; i <= 90; i++) {
        float theta = (darkAngleDeg + i) * PI / 180.0f;
        glVertex2f(radius * cos(theta), radius * sin(theta));
    }

    glEnd();
}

void drawEarth() {
    drawGlow(0, 0, 0.17f, 40, 130, 255);

    glColor3ub(40, 95, 215);
    drawCircle(0, 0, 0.16f, 120);

    glPushMatrix();
    glRotatef(earthSpin, 0, 0, 1);

    glColor3ub(45, 155, 65);
    glBegin(GL_POLYGON);
    glVertex2f(-0.07f,  0.04f);
    glVertex2f(-0.02f,  0.10f);
    glVertex2f( 0.03f,  0.07f);
    glVertex2f( 0.01f,  0.01f);
    glVertex2f(-0.05f, -0.02f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.04f,  0.08f);
    glVertex2f(0.11f,  0.12f);
    glVertex2f(0.13f,  0.04f);
    glVertex2f(0.08f, -0.01f);
    glVertex2f(0.03f,  0.02f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.02f, -0.02f);
    glVertex2f( 0.03f, -0.08f);
    glVertex2f( 0.01f, -0.14f);
    glVertex2f(-0.06f, -0.10f);
    glVertex2f(-0.08f, -0.04f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.06f, -0.07f);
    glVertex2f(0.11f, -0.09f);
    glVertex2f(0.12f, -0.12f);
    glVertex2f(0.08f, -0.14f);
    glVertex2f(0.04f, -0.11f);
    glEnd();

    glPopMatrix();

    glColor4f(0.65f, 0.85f, 1.0f, 0.45f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 140; i++) {
        float theta = 2.0f * PI * i / 140.0f;
        glVertex2f(0.162f * cos(theta), 0.162f * sin(theta));
    }
    glEnd();

    float earthWorldX = 0.0f;
    float earthWorldY = -0.08f;
    drawDirectionalShadow(0.16f, getDarkSideAngle(earthWorldX, earthWorldY), 0.38f);
}

void drawMoonBody(float localShadowAngle) {
    drawGlow(0, 0, 0.045f, 210, 215, 235);

    glColor3ub(205, 208, 218);
    drawCircle(0, 0, 0.045f, 70);

    glColor3ub(150, 155, 170);
    drawCircle(-0.014f,  0.012f, 0.009f, 25);
    drawCircle( 0.010f, -0.007f, 0.006f, 25);
    drawCircle(-0.004f, -0.020f, 0.005f, 25);

    drawDirectionalShadow(0.045f, localShadowAngle, 0.32f);
}

void drawEarthMoonSystem() {
    glPushMatrix();
    glTranslatef(0.0f, -0.08f, 0.0f);

    drawOrbitLine(0.31f);
    drawEarth();

    float moonOrbitRadius = 0.31f;
    float moonAngleRad = moonOrbit * PI / 180.0f;
    float earthWorldX = 0.0f;
    float earthWorldY = -0.08f;
    float moonWorldX = earthWorldX + moonOrbitRadius * cos(moonAngleRad);
    float moonWorldY = earthWorldY + moonOrbitRadius * sin(moonAngleRad);
    float moonWorldShadowAngle = getDarkSideAngle(moonWorldX, moonWorldY);
    float moonLocalShadowAngle = moonWorldShadowAngle - moonOrbit;

    glPushMatrix();
    glRotatef(moonOrbit, 0, 0, 1);
    glTranslatef(moonOrbitRadius, 0.0f, 0.0f);
    drawMoonBody(moonLocalShadowAngle);
    glPopMatrix();

    glPopMatrix();
}

void drawMapMarker(float x, float y) {
    glColor3ub(70, 120, 255);
    drawCircle(x, y, 0.028f, 40);

    glColor4f(70 / 255.0f, 120 / 255.0f, 1.0f, 0.25f);
    drawCircle(x, y, 0.050f, 50);
}

void drawGoogleStyleControls() {
}

void drawOutlinePoints(const float points[][2], int count) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < count; i++) {
        glVertex2f(points[i][0], points[i][1]);
    }
    glEnd();
}

void drawMapLandShape(float shade) {
    glColor3f(0.12f * shade, 0.62f * shade, 0.18f * shade);

    glBegin(GL_POLYGON);
    glVertex2f(-1.72f, 0.66f); glVertex2f(-1.42f, 0.80f);
    glVertex2f(-1.08f, 0.72f); glVertex2f(-0.90f, 0.52f);
    glVertex2f(-1.00f, 0.32f); glVertex2f(-1.18f, 0.18f);
    glVertex2f(-1.08f, 0.04f); glVertex2f(-1.34f, 0.10f);
    glVertex2f(-1.56f, 0.28f);
    glEnd();

    glColor3f(0.13f * shade, 0.58f * shade, 0.18f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(-0.94f, 0.88f); glVertex2f(-0.62f, 0.98f);
    glVertex2f(-0.42f, 0.84f); glVertex2f(-0.56f, 0.68f);
    glVertex2f(-0.88f, 0.70f);
    glEnd();

    glColor3f(0.10f * shade, 0.58f * shade, 0.17f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(-1.10f, 0.02f); glVertex2f(-0.84f, -0.08f);
    glVertex2f(-0.74f, -0.34f); glVertex2f(-0.82f, -0.58f);
    glVertex2f(-0.70f, -0.78f); glVertex2f(-0.82f, -0.90f);
    glVertex2f(-0.98f, -0.64f); glVertex2f(-1.10f, -0.34f);
    glEnd();

    glColor3f(0.13f * shade, 0.61f * shade, 0.18f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(-0.30f, 0.62f); glVertex2f(0.08f, 0.78f);
    glVertex2f(0.55f, 0.72f); glVertex2f(1.10f, 0.68f);
    glVertex2f(1.70f, 0.58f); glVertex2f(1.84f, 0.30f);
    glVertex2f(1.50f, 0.12f); glVertex2f(1.18f, 0.02f);
    glVertex2f(1.02f, -0.16f); glVertex2f(0.72f, -0.02f);
    glVertex2f(0.48f, 0.12f); glVertex2f(0.16f, 0.20f);
    glVertex2f(-0.08f, 0.34f);
    glEnd();

    glColor3f(0.17f * shade, 0.56f * shade, 0.15f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(0.02f, 0.26f); glVertex2f(0.34f, 0.20f);
    glVertex2f(0.50f, -0.10f); glVertex2f(0.42f, -0.48f);
    glVertex2f(0.28f, -0.78f); glVertex2f(0.02f, -0.62f);
    glVertex2f(-0.12f, -0.20f); glVertex2f(-0.16f, 0.10f);
    glEnd();

    glColor3f(0.12f * shade, 0.56f * shade, 0.15f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(1.20f, -0.48f); glVertex2f(1.55f, -0.46f);
    glVertex2f(1.72f, -0.66f); glVertex2f(1.45f, -0.82f);
    glVertex2f(1.12f, -0.68f);
    glEnd();

    glColor3f(0.72f * shade, 0.48f * shade, 0.33f * shade);
    glBegin(GL_POLYGON);
    glVertex2f(-1.95f, -0.92f); glVertex2f(-1.15f, -0.86f);
    glVertex2f(-0.35f, -0.94f); glVertex2f(0.55f, -0.88f);
    glVertex2f(1.40f, -0.96f); glVertex2f(1.95f, -0.90f);
    glVertex2f(1.95f, -1.10f); glVertex2f(-1.95f, -1.10f);
    glEnd();

    glColor4f(0.90f, 0.63f, 0.27f, 0.42f);
    drawEllipse(0.18f, 0.02f, 0.24f, 0.10f, 60);
    drawEllipse(0.70f, 0.39f, 0.28f, 0.08f, 60);
    drawEllipse(1.45f, -0.60f, 0.16f, 0.06f, 40);

    glColor4f(0.40f, 0.24f, 0.14f, 0.48f);
    drawEllipse(-1.54f, 0.38f, 0.040f, 0.24f, 50);
    drawEllipse(-0.92f, -0.42f, 0.035f, 0.34f, 50);
    drawEllipse(0.86f, 0.20f, 0.22f, 0.032f, 50);
    drawEllipse(0.20f, -0.42f, 0.052f, 0.20f, 50);
}

void drawCountryBorders() {
}

void drawMapCoastOutlines() {
}

void drawMapLabels(bool zoomed) {
}

void drawOceanRelief() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.60f, 0.82f, 1.0f, 0.16f);
    glLineWidth(1.0f);

    for (int row = 0; row < 12; row++) {
        float y = -0.86f + row * 0.15f;
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 120; i++) {
            float x = -1.95f + i * (3.90f / 120.0f);
            float wave = 0.018f * sin(i * 0.28f + row * 0.70f);
            glVertex2f(x, y + wave);
        }
        glEnd();
    }

    glColor4f(0.02f, 0.10f, 0.30f, 0.24f);
    drawEllipse(-1.45f, -0.22f, 0.38f, 0.20f, 80);
    drawEllipse(-0.18f, -0.52f, 0.42f, 0.20f, 80);
    drawEllipse(1.22f, -0.12f, 0.46f, 0.24f, 80);
}

void drawMapContent(bool zoomed) {
    drawOceanRelief();
    drawMapLandShape(1.0f);
    drawCountryBorders();
    drawMapCoastOutlines();
    drawMapLabels(zoomed);
}

void drawMapScene(bool zoomed) {
    glClearColor(0.03f, 0.17f, 0.36f, 1.0f);

    glColor3ub(28, 82, 139);
    drawRect(-2.0f, -1.2f, 2.0f, 1.2f);

    glPushMatrix();

    if (zoomed) {
        glScalef(mapScale, mapScale, 1.0f);
        glTranslatef(-0.78f, -0.02f, 0.0f);
    } else {
        glScalef(mapScale, mapScale, 1.0f);
    }

    drawMapContent(zoomed);
    glPopMatrix();

}

void display() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (sceneMode == 1) {
        drawMapScene(false);
    } else if (sceneMode == 2) {
        drawMapScene(true);
    } else {
        drawStars();
        drawTopBar();
        drawSun();
        drawLensFlare();
        drawEarthMoonSystem();

        glColor3ub(220, 225, 245);
      //  drawText(-1.15f, -0.92f, "Earth-Moon System   1: wide map   2: scaled map   R: run   S: stop   + / -: speed");
    }

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    if (w >= h) {
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    } else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':
        sceneMode = 1;
        mapScale = 1.0f;
        break;
    case '2':
        sceneMode = 2;
        mapScale = 2.20f;
        break;
    case '3':
        sceneMode = 3;
        break;
    case 'r':
    case 'R':
        running = true;
        break;
    case 's':
    case 'S':
        running = false;
        break;
    case '+':
    case '=':
        if (sceneMode == 1 || sceneMode == 2) {
            mapScale += 0.15f;
            if (mapScale > 4.0f) mapScale = 4.0f;
        } else {
            speedValue += 0.2f;
            if (speedValue > 5.0f) speedValue = 5.0f;
        }
        break;
    case '-':
        if (sceneMode == 1 || sceneMode == 2) {
            mapScale -= 0.15f;
            if (mapScale < 0.6f) mapScale = 0.6f;
        } else {
            speedValue -= 0.2f;
            if (speedValue < 0.2f) speedValue = 0.2f;
        }
        break;
    case 'q':
    case 'Q':
        if (sceneMode == 1) mapScale = 1.0f;
        if (sceneMode == 2) mapScale = 2.20f;
        break;
    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

void update(int value) {
    if (running) {
        earthSpin += 1.2f * speedValue;
        moonOrbit += 0.8f * speedValue;

        if (earthSpin > 360.0f) earthSpin -= 360.0f;
        if (moonOrbit > 360.0f) moonOrbit -= 360.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Earth Moon System - Proper Moon Rotation");

    glEnable(GL_BLEND);
    initStars();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
