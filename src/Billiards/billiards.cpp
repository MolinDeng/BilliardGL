#define FREEGLUT_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#include "ObjLoader.h"
#include "particle.h"
#include "GL/freeglut.h"
#include <thread>
#include <GL/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, GL_Lib("freeglut_static"))
#pragma comment(lib, GL_Lib("glew"))
#pragma comment(lib,"winmm.lib")

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define TABLE_OUT_WIDTH 153
#define TABLE_OUT_LENGTH 281
#define TABLE_IN_WIDTH 124.5
#define TABLE_IN_LENGTH 252
#define TABLE_HEIGHT 87
#define CUE_LENGTH 145
#define POCKET_RADIUS 8.5
#define ROOM_WIDTH 1000
#define ROOM_LENGTH 1000
#define ROOM_HEIGHT 400
#define BALL_RADIUS 5.715

#define BMP_Header_Length 54
#define PI 3.1415926
#define L0 GL_LIGHT0
#define L1 GL_LIGHT1

GLuint texture[10];
emitter *e[16];

//变量申明
GLint  width = 1024, height = 768;
GLuint texGround, texWall, texWall1, texWall2, tecCeiling, texTableCloth, texTable, BZD, texCue, texgt, texhe;
int mx = 0, my = 0, i = 0, j = 0, k = 0, ballcnt = 16, BallInNum = 0, AimAt = 0, WaitHit = 0, Hit = 0;
int leftm = 0, rightm = 0;
GLfloat rx, ry, rz, speed = 0;
static GLfloat kx = 0, ky = 0, kz = 0, zoom = 120;
static GLfloat anglex = -PI / 2, angley = PI / 3, nowanglex = 0, nowangley = 0, nowatx = 0, nowaty = 0;
static GLfloat M = 1, U = 0.2, T = 0.1, Radius = 5.715, G = -4;
static GLfloat at[6] = { 0, 200, -TABLE_IN_LENGTH / 4, 0, TABLE_HEIGHT + Radius, -TABLE_IN_LENGTH / 4 };
GLfloat m[16];
GLuint tableVertexVBO, cueVertexVBO, benchVertexVBO, wardVertexVBO, paint1VertexVBO;
GLuint textureIDtest[2];
GLuint textureCue[2], textureWard, texturePaint1, texturePaint2;
ObjLoader tableObj(".//obj//table.obj");
ObjLoader cueObj(".//obj//cue.obj");
ObjLoader benchObj(".//obj//bench.obj");
ObjLoader wardObj(".//obj//wardrobe.obj");

bool IsMoving = FALSE;
bool transPerc = FALSE;
bool Isrecroded = FALSE;
float record_zoom;
float record_position[3];
int PlayerBall[2];// 0代表纯色球 1-7  1代表花色球 9-15
int IsFirstInBall = 1;
int CurrPlayer = 0;
int NextPlayer = 0;
int IsIllegal = 0;
bool updated = FALSE;
bool Hitted = FALSE;
bool IsGameOver = FALSE;
bool Fired[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
bool AllFired = FALSE;

//定义球及位置矢量结构体
struct Point
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct Ball
{
	struct Point p;
	struct Point v;
	struct Point a;
	GLfloat mv;
	GLfloat ma;
	int isIn;
	GLuint texture;
} Ball[16];

// 载入纹理
int isPowerOfTwo(int n);
GLuint loadTexture(const char* file_name);
void initLoadTexture();
// 场景绘制
void initBall();
void initWindows(void);
void initDecoration();
void myDisplay(void);
void set_camera(void);
void renderRoom(void);
void renderTable(void);
void renderBall(void);
void drawBall(GLfloat mybr, GLuint BALL);
void renderCue();
void renderRect();
void renderDecoration();
void myIdle(void);
void collideBalls(int j, int k);
void collideEdge(int j);
int isBallIn(int j);
void drawString();
void selectFont(int size, int charset, const char* face);
void myString(float x, float y, void *font, char* c);
void updatePlayer();
void initTable();
void initCue();
void setMaterial(Material *mat);
// 光源
void initLight();
// 鼠标键盘操作
static void myKeyboard(unsigned char key, int x, int y);
static void myMouse(int mbutton, int mstate, int x, int y);
static void mouseMove(int x, int y);
void b_music();

particle* init_flame()
{
	float size = rand() % 90 * 0.02f;
	float speed[] = { float(rand() % 10 - 4) / 1600, float(rand() % 10 - 4) / 800, float(rand() % 10 - 4) / 1600 };
	float acc[] = { 1.0f*(rand() % 3 - 1) / 9000000,4.9 / 4000000 ,1.0f*(rand() % 3 - 1) / 9000000 };
	float angle[] = { rand() % 360, rand() % 360 ,rand() % 360 };
	particle* p = new particle(vec(size, size, size), vec(speed), vec(acc),
		vec(angle), rand() % 50 + 10, texture[2]);
	return p;
}


int main(int argc, char* argv[])
{
	std::thread t(b_music);
	t.detach();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STENCIL);
	initWindows();
	initBall();//初始化球的位置
	initLoadTexture();
	initCue();
	initTable();
	initDecoration();

	for (int i = 0; i < ballcnt; i++)
	{
		//		if(i==0)
		e[i] = new emitter(init_flame, 5000, -Radius + Ball[i].p.x, Radius + Ball[i].p.x, Ball[i].p.y, Ball[i].p.y, Ball[i].p.z, Ball[i].p.z);
	}

	glutDisplayFunc(&myDisplay);
	glutIdleFunc(&myIdle); //设置窗口刷新的回调函数
	glutKeyboardFunc(myKeyboard); //设置键盘回调函数
	glutMouseFunc(myMouse); //设置鼠标器按键回调函数
	glutMotionFunc(mouseMove); //设置鼠标器移动回调函数

	initLight(); // 光照模型
	glutMainLoop();
	return 0;
}

// 初始化窗口
void initWindows(void)
{
	glutCreateWindow("Billards");
	glutFullScreen();
}
// 初始化球位置
void initBall()
{
	Point s;
	s.x = 0;
	s.z = TABLE_IN_LENGTH / 4;
	s.y = TABLE_HEIGHT + BALL_RADIUS;
	float y_dis = sqrt(3)*BALL_RADIUS;
	float x_dis = BALL_RADIUS;
	//Ball[0] is the white ball
	Ball[0].p.x = s.x; Ball[0].p.z = -s.z; Ball[0].p.y = s.y;
	//first row
	Ball[1].p.x = s.x; Ball[1].p.z = s.z; Ball[1].p.y = s.y;
	//second row
	Ball[2].p.x = s.x - x_dis; Ball[2].p.z = s.z + y_dis; Ball[2].p.y = s.y;
	Ball[3].p.x = s.x + x_dis; Ball[3].p.z = s.z + y_dis; Ball[3].p.y = s.y;

	Ball[4].p.x = s.x - 2 * x_dis; Ball[4].p.z = s.z + 2 * y_dis; Ball[4].p.y = s.y;
	Ball[8].p.x = s.x; Ball[8].p.z = s.z + 2 * y_dis; Ball[8].p.y = s.y;
	Ball[6].p.x = s.x + 2 * x_dis; Ball[6].p.z = s.z + 2 * y_dis; Ball[6].p.y = s.y;

	Ball[7].p.x = s.x - 3 * x_dis; Ball[7].p.z = s.z + 3 * y_dis; Ball[7].p.y = s.y;
	Ball[5].p.x = s.x - x_dis; Ball[5].p.z = s.z + 3 * y_dis; Ball[5].p.y = s.y;
	Ball[9].p.x = s.x + x_dis; Ball[9].p.z = s.z + 3 * y_dis; Ball[9].p.y = s.y;
	Ball[10].p.x = s.x + 3 * x_dis; Ball[10].p.z = s.z + 3 * y_dis; Ball[10].p.y = s.y;

	Ball[11].p.x = s.x - 4 * x_dis; Ball[11].p.z = s.z + 4 * y_dis; Ball[11].p.y = s.y;
	Ball[12].p.x = s.x - 2 * x_dis; Ball[12].p.z = s.z + 4 * y_dis; Ball[12].p.y = s.y;
	Ball[13].p.x = s.x; Ball[13].p.z = s.z + 4 * y_dis; Ball[13].p.y = s.y;
	Ball[14].p.x = s.x + 2 * x_dis; Ball[14].p.z = s.z + 4 * y_dis; Ball[14].p.y = s.y;
	Ball[15].p.x = s.x + 4 * x_dis; Ball[15].p.z = s.z + 4 * y_dis; Ball[15].p.y = s.y;

	for (i = 0; i < ballcnt; i++)
	{
		Ball[i].v.x = 0; Ball[i].v.z = 0; Ball[i].v.y = 0;
		Ball[i].a.x = 0; Ball[i].a.z = 0; Ball[i].a.y = 0;
		Ball[i].isIn = 0;
		Ball[i].mv = 0;
		Ball[i].ma = 0;
	}
}

void initTable() {
	glEnable(GL_TEXTURE_2D);
	textureIDtest[0] = loadTexture((".//tex//" + tableObj.materials[0]->texture).c_str());
	textureIDtest[1] = loadTexture((".//tex//" + tableObj.materials[1]->texture).c_str());

	glewInit();
	GLfloat *tableVertex;
	GLfloat *tableNormal;
	tableNormal = new GLfloat[tableObj.vertices.size() * 3];
	tableVertex = new GLfloat[tableObj.vertices.size() * 3];
	GLfloat *tableTexture = new GLfloat[tableObj.vertices.size() * 3];
	for (int i = 0; i < tableObj.vertices.size(); i++) {
		tableVertex[i * 3] = tableObj.vertices[i].position.x;
		tableVertex[i * 3 + 1] = tableObj.vertices[i].position.y;
		tableVertex[i * 3 + 2] = tableObj.vertices[i].position.z;
		tableNormal[i * 3] = tableObj.vertices[i].normal.x;
		tableNormal[i * 3 + 1] = tableObj.vertices[i].normal.y;
		tableNormal[i * 3 + 2] = tableObj.vertices[i].normal.z;
		tableTexture[i * 3] = tableObj.vertices[i].texture.x;
		tableTexture[i * 3 + 1] = tableObj.vertices[i].texture.y;
		tableTexture[i * 3 + 2] = tableObj.vertices[i].texture.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	size_t dataSize = sizeof(GLfloat) * tableObj.vertices.size() * 3;
	glGenBuffersARB(1, &tableVertexVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, tableVertexVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 3, 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, dataSize, tableVertex);                             // copy vertices starting from 0 offest
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize, dataSize, tableNormal);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 2, dataSize, tableTexture);
}
void initDecoration() {
	glEnable(GL_TEXTURE_2D);
	glewInit();
	GLfloat *benchVertex;
	GLfloat *benchNormal;
	benchNormal = new GLfloat[benchObj.vertices.size() * 3];
	benchVertex = new GLfloat[benchObj.vertices.size() * 3];
	GLfloat *benchTexture = new GLfloat[benchObj.vertices.size() * 3];
	for (int i = 0; i < benchObj.vertices.size(); i++) {
		benchVertex[i * 3] = benchObj.vertices[i].position.x / 3;
		benchVertex[i * 3 + 1] = benchObj.vertices[i].position.y / 3;
		benchVertex[i * 3 + 2] = benchObj.vertices[i].position.z / 3;
		benchNormal[i * 3] = benchObj.vertices[i].normal.x;
		benchNormal[i * 3 + 1] = benchObj.vertices[i].normal.y;
		benchNormal[i * 3 + 2] = benchObj.vertices[i].normal.z;
		benchTexture[i * 3] = benchObj.vertices[i].texture.x;
		benchTexture[i * 3 + 1] = benchObj.vertices[i].texture.y;
		benchTexture[i * 3 + 2] = benchObj.vertices[i].texture.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	size_t dataSize = sizeof(GLfloat) * benchObj.vertices.size() * 3;
	glGenBuffersARB(1, &benchVertexVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, benchVertexVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 3, 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, dataSize, benchVertex);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize, dataSize, benchNormal);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 2, dataSize, benchTexture);

	glEnable(GL_TEXTURE_2D);
	glewInit();
	GLfloat *wardVertex;
	GLfloat *wardNormal;
	wardNormal = new GLfloat[wardObj.vertices.size() * 3];
	wardVertex = new GLfloat[wardObj.vertices.size() * 3];
	GLfloat *wardTexture = new GLfloat[wardObj.vertices.size() * 3];
	for (int i = 0; i < wardObj.vertices.size(); i++) {
		wardVertex[i * 3] = wardObj.vertices[i].position.x * 3;
		wardVertex[i * 3 + 1] = wardObj.vertices[i].position.y * 3;
		wardVertex[i * 3 + 2] = wardObj.vertices[i].position.z * 3;
		wardNormal[i * 3] = wardObj.vertices[i].normal.x;
		wardNormal[i * 3 + 1] = wardObj.vertices[i].normal.y;
		wardNormal[i * 3 + 2] = wardObj.vertices[i].normal.z;
		wardTexture[i * 3] = wardObj.vertices[i].texture.x;
		wardTexture[i * 3 + 1] = wardObj.vertices[i].texture.y;
		wardTexture[i * 3 + 2] = wardObj.vertices[i].texture.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	dataSize = sizeof(GLfloat) * wardObj.vertices.size() * 3;
	glGenBuffersARB(1, &wardVertexVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, wardVertexVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 3, 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, dataSize, wardVertex);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize, dataSize, wardNormal);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 2, dataSize, wardTexture);


	/*glEnable(GL_TEXTURE_2D);
	glewInit();
	GLfloat *paint1Vertex;
	GLfloat *paint1Normal;
	paint1Normal = new GLfloat[paint1Obj.vertices.size() * 3];
	paint1Vertex = new GLfloat[paint1Obj.vertices.size() * 3];
	GLfloat *paint1Texture = new GLfloat[paint1Obj.vertices.size() * 3];
	for (int i = 0; i < paint1Obj.vertices.size(); i++) {
	paint1Vertex[i * 3] = paint1Obj.vertices[i].position.x / 5;
	paint1Vertex[i * 3 + 1] = paint1Obj.vertices[i].position.y / 5;
	paint1Vertex[i * 3 + 2] = paint1Obj.vertices[i].position.z / 5;
	paint1Normal[i * 3] = paint1Obj.vertices[i].normal.x;
	paint1Normal[i * 3 + 1] = paint1Obj.vertices[i].normal.y;
	paint1Normal[i * 3 + 2] = paint1Obj.vertices[i].normal.z;
	paint1Texture[i * 3] = paint1Obj.vertices[i].texture.x;
	paint1Texture[i * 3 + 1] = paint1Obj.vertices[i].texture.y;
	paint1Texture[i * 3 + 2] = paint1Obj.vertices[i].texture.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	dataSize = sizeof(GLfloat) * paint1Obj.vertices.size() * 3;
	glGenBuffersARB(1, &paint1VertexVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, paint1VertexVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 3, 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, dataSize, paint1Vertex);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize, dataSize, paint1Normal);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 2, dataSize, paint1Texture);*/
}
void initCue() {
	glEnable(GL_TEXTURE_2D);
	textureCue[0] = loadTexture((".//tex//" + cueObj.materials[0]->texture).c_str());
	textureCue[1] = loadTexture((".//tex//" + cueObj.materials[1]->texture).c_str());
	cout << cueObj.materials[1]->texture << endl;
	glewInit();
	GLfloat *cueVertex;
	GLfloat *cueNormal;
	cueNormal = new GLfloat[cueObj.vertices.size() * 3];
	cueVertex = new GLfloat[cueObj.vertices.size() * 3];
	GLfloat *cueTexture = new GLfloat[cueObj.vertices.size() * 3];
	for (int i = 0; i < cueObj.vertices.size(); i++) {
		cueVertex[i * 3] = cueObj.vertices[i].position.x;
		cueVertex[i * 3 + 1] = cueObj.vertices[i].position.y;
		cueVertex[i * 3 + 2] = cueObj.vertices[i].position.z;
		cueNormal[i * 3] = cueObj.vertices[i].normal.x;
		cueNormal[i * 3 + 1] = cueObj.vertices[i].normal.y;
		cueNormal[i * 3 + 2] = cueObj.vertices[i].normal.z;
		cueTexture[i * 3] = cueObj.vertices[i].texture.x;
		cueTexture[i * 3 + 1] = cueObj.vertices[i].texture.y;
		cueTexture[i * 3 + 2] = cueObj.vertices[i].texture.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	size_t dataSize = sizeof(GLfloat) * cueObj.vertices.size() * 3;
	glGenBuffersARB(1, &cueVertexVBO);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, cueVertexVBO);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 3, 0, GL_STATIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, dataSize, cueVertex);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize, dataSize, cueNormal);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, dataSize * 2, dataSize, cueTexture);
}

void setMaterial(Material *mat) {
	const GLfloat a[4] = { mat->ambient[0], mat->ambient[1],mat->ambient[2],1.0f };
	const GLfloat d[4] = { mat->diffuse[0],mat->diffuse[1],mat->diffuse[2],1.0f };
	const GLfloat s[4] = { mat->specular[0],mat->specular[1],mat->specular[2],1.0f };
	//const GLfloat e[4] = { mat.emission[0],mat.emission[1],mat.emission[2],1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, a);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s);
	//glMaterialfv(GL_FRONT, GL_EMISSION, e);
	glMaterialf(GL_FRONT, GL_SHININESS, mat->nShininess);
}
// display
void myDisplay(void)
{
	set_camera();
	renderRoom();

	//	glEnable(GL_CULL_FACE);
	renderTable();
	renderBall();
	renderDecoration();

	if (AimAt == 1)
		renderCue();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

	for (int i = 0; i < 16; i++)
	{
		if (Fired[i] || AllFired)
		{
			e[i]->emit(-Radius + Ball[i].p.x, Radius + Ball[i].p.x, Ball[i].p.y, Ball[i].p.y, Ball[i].p.z, Ball[i].p.z);
			e[i]->show();
		}
	}

	//e->update();

	glDisable(GL_BLEND);


	if (WaitHit == 1)
		renderRect();

	updatePlayer();
	drawString();
	glutSwapBuffers();
}
// 设置视点
void set_camera(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 16.0 / 9, 10, 10000.0f);
	glTranslatef(kx, ky, kz);
	glMatrixMode(GL_TEXTURE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(at[0], at[1], at[2], at[3], at[4], at[5], 0.0, 1.0, 0.0);//0.0,300.0,500.0,0.0,80.0,0.0
}
// 画房间
void renderRoom()
{
	// 使用 " 地"纹理绘制土地
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texGround);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glEnd();
	//天花板
	glBindTexture(GL_TEXTURE_2D, tecCeiling);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 5.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(5.0f, 5.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(5.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glEnd();
	// 使用 " 墙"纹理绘制栅栏
	glBindTexture(GL_TEXTURE_2D, texWall1);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texWall2);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texWall);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texWall);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, 0, ROOM_LENGTH / 2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(ROOM_WIDTH / 2, ROOM_HEIGHT, -ROOM_LENGTH / 2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(ROOM_WIDTH / 2, 0, -ROOM_LENGTH / 2);
	glEnd();
}
// 画桌子
void renderTable()
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, tableVertexVBO);

	// enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// before draw, specify vertex and index arrays with their offsets
	glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(GLfloat) * tableObj.vertices.size() * 3));
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(3, GL_FLOAT, 0, (void*)((sizeof(GLfloat) * tableObj.vertices.size() * 3) * 2));

	//setMaterial(tableObj.materials[tableObj.mtlIndex[1]]);
	glBindTexture(GL_TEXTURE_2D, textureIDtest[tableObj.mtlIndex[1]]);
	glDrawArrays(GL_TRIANGLES, 0, tableObj.mtlIndex[4]);

	//setMaterial(tableObj.materials[tableObj.mtlIndex[5]]);
	glBindTexture(GL_TEXTURE_2D, textureIDtest[tableObj.mtlIndex[5]]);
	glDrawArrays(GL_TRIANGLES, tableObj.mtlIndex[4], tableObj.mtlIndex[6] / 2);

	//setMaterial(tableObj.materials[tableObj.mtlIndex[1]]);
	glBindTexture(GL_TEXTURE_2D, textureIDtest[tableObj.mtlIndex[1]]);
	glDrawArrays(GL_TRIANGLES, tableObj.mtlIndex[6] / 2, tableObj.vertices.size());

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// it is good idea to release VBOs with ID 0 after use.
	// Once bound with 0, all pointers in gl*Pointer() behave as real
	// pointer, so, normal vertex array operations are re-activated
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void renderDecoration() {
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, benchVertexVBO);

	// enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// before draw, specify vertex and index arrays with their offsets
	glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(GLfloat) * benchObj.vertices.size() * 3));
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(3, GL_FLOAT, 0, (void*)((sizeof(GLfloat) * benchObj.vertices.size() * 3) * 2));

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, -ROOM_WIDTH / 2 + 50);
	glBindTexture(GL_TEXTURE_2D, texhe);
	//setMaterial(benchObj.materials[benchObj.mtlIndex[0]]);
	glDrawArrays(GL_TRIANGLES, 0, benchObj.vertices.size());
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);




	glBindBufferARB(GL_ARRAY_BUFFER_ARB, wardVertexVBO);

	// enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// before draw, specify vertex and index arrays with their offsets
	glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(GLfloat) * wardObj.vertices.size() * 3));
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(3, GL_FLOAT, 0, (void*)((sizeof(GLfloat) * wardObj.vertices.size() * 3) * 2));

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, -ROOM_WIDTH / 2 + 30);
	glBindTexture(GL_TEXTURE_2D, textureWard);
	//setMaterial(wardObj.materials[wardObj.mtlIndex[0]]);
	glDrawArrays(GL_TRIANGLES, 0, wardObj.vertices.size());
	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

}
// 设置绘球参数
void renderBall(void)
{
	for (i = 0; i<ballcnt; i++) {
		if (Ball[i].isIn == 0) {
			for (j = 0; j < 15; j++) m[j] = 0.0;
			m[0] = m[5] = m[10] = 1.0;
			m[7] = (-1.0) / 405;
			glPushMatrix();
			glTranslatef(Ball[i].p.x, Ball[i].p.y, Ball[i].p.z);
			glPushMatrix();
			glTranslatef(0, 400, 0);
			glMultMatrixf(m);
			glTranslatef(0, -400, 0);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glColor4f(0.3, 0.3, 0.3, 0.5);
			glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
			drawBall(Radius, texhe);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glPopMatrix();
			glRotatef(Ball[i].ma, Ball[i].a.x, Ball[i].a.y, Ball[i].a.z);
			//			glEnable(GL_BLEND);
			//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			drawBall(Radius, Ball[i].texture);
			//			glDisable(GL_BLEND);
			glPopMatrix();
		}
		else {
			glPushMatrix();
			glTranslatef(Ball[i].p.x, Ball[i].p.y, Ball[i].p.z);
			drawBall(Radius, Ball[i].texture);
			glPopMatrix();
		}
	}
}
// 画球
void drawBall(GLfloat mybr, GLuint BALL)
{
	GLUquadricObj *MYBALL;
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, BALL);
		glBegin(GL_QUADS);
		MYBALL = gluNewQuadric();// 创建一个二次曲面物体
		gluQuadricTexture(MYBALL, GL_TRUE); //启用该二次曲面的纹理
		glBindTexture(GL_TEXTURE_2D, BALL);// 绑定纹理
		gluSphere(MYBALL, mybr, 80, 120); // 画球
		gluDeleteQuadric(MYBALL);
		glEnd();
	}
}
// 画球杆、力度条、方向线
void renderCue()
{
	GLfloat lxz;
	lxz = sqrt(pow(at[0] - at[3], 2) + pow(at[2] - at[5], 2));

	glPushMatrix();
	glTranslatef(Ball[0].p.x, Ball[0].p.y, Ball[0].p.z);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(-150 * (at[0] - at[3]) / lxz, 0, -150 * (at[2] - at[5]) / lxz);
	glEnd();
	glPopMatrix();
	//在这个函数里画球杆

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, cueVertexVBO);

	// enable vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// before draw, specify vertex and index arrays with their offsets
	glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(GLfloat) * cueObj.vertices.size() * 3));
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(3, GL_FLOAT, 0, (void*)((sizeof(GLfloat) * cueObj.vertices.size() * 3) * 2));

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(Ball[0].p.x + (speed + 6) * 0.1 * (at[0] - at[3]) / lxz, Ball[0].p.y, Ball[0].p.z + (speed + 6) * 0.1 * (at[2] - at[5]) / lxz);
	/*float theta = acosf(cos(anglex) / sin(angley)) * 180 / PI;
	if (anglex < -PI) theta = 360 - theta;
	else if (anglex > 0) theta *= -1;
	cout << theta << endl;*/

	glRotatef(180.5 - 90 - anglex * 180 / PI, 0, 1, 0);
	//glRotatef(80 - angley * 180 / PI, (at[2] - at[5]) / lxz, 0, -(at[0] - at[3]) / lxz);
	//setMaterial(cueObj.materials[0]);

	glBindTexture(GL_TEXTURE_2D, textureCue[0]);
	glDrawArrays(GL_TRIANGLES, cueObj.mtlIndex[4], cueObj.vertices.size());
	glBindTexture(GL_TEXTURE_2D, textureCue[1]);
	glDrawArrays(GL_TRIANGLES, cueObj.mtlIndex[2], cueObj.mtlIndex[4]);
	glBindTexture(GL_TEXTURE_2D, Ball[0].texture);
	glDrawArrays(GL_TRIANGLES, 0, cueObj.mtlIndex[2]);


	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void renderRect()
{
	//to2d,画力度条
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1024, 0, 768);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glColor4f(1.0, 1.0, 0.0, 0.1);
	glBlendFunc(GL_ONE, GL_ZERO);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, BZD);
	glRectf(10, 500, 50, speed + 500);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glPopMatrix();
	//to3d,貌似没用
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
// 更新位置、球数量、速度
void myIdle(void)
{
	GLfloat vx = 0, vz = 0;
	at[0] = zoom*(cos(anglex)) + at[3];
	at[1] = zoom*(cos(angley)) + at[4];
	at[2] = zoom*(sin(anglex) * sin(angley)) + at[5];
	bool flag = FALSE;
	for (i = 0; i<ballcnt; i++)
	{
		if (Ball[i].isIn == 0)
		{
			for (k = i + 1; k<ballcnt; k++) collideBalls(i, k);// 碰撞检测
			collideEdge(i);
			isBallIn(i);
			Ball[i].mv = pow(Ball[i].v.x, 2) + pow(Ball[i].v.z, 2);
			if (Ball[i].mv>0.1)
			{
				Ball[i].mv = sqrt(Ball[i].mv);
				vx = Ball[i].v.x / Ball[i].mv;
				vz = Ball[i].v.z / Ball[i].mv;
				Ball[i].mv += G*T;
				Ball[i].v.x = Ball[i].mv*vx;
				Ball[i].v.z = Ball[i].mv*vz;
				Ball[i].p.x += Ball[i].v.x*T;
				Ball[i].p.z += Ball[i].v.z*T;
				Ball[i].a.x = (-1)*vz;
				Ball[i].a.z = vx;
				Ball[i].ma += -180 * Ball[i].mv*T / (Radius*PI);
				flag = TRUE;
			}
			else Ball[i].mv = 0;
		}
	}
	if (!flag)
	{
		IsMoving = FALSE;
	}
	else
		IsMoving = TRUE;
	if (WaitHit == 1)
	{
		if (speed>200) speed -= 200;
		else speed += 2;
	}
	if (Hit == 1)
	{
		GLfloat atxy = 0;
		atxy = sqrt(pow(at[3] - at[0], 2) + pow(at[5] - at[2], 2));
		Ball[0].v.x = speed*(at[3] - at[0]) / atxy;
		Ball[0].v.z = speed*(at[5] - at[2]) / atxy;
		Hit = 0;
		for (i = speed * 100; i > 0; i--) {
			speed -= 0.01;
		}
		mciSendString(TEXT("play audio//hit.wav"), NULL, 0, NULL);
	}
	if (leftm)
	{
		Ball[0].v.x = 0;
		Ball[0].v.z = 0;
		speed = 0;
	}
	if (transPerc)
	{
		if (!Isrecroded)
		{
			record_zoom = zoom;
			record_position[0] = Ball[0].p.x;
			record_position[1] = Ball[0].p.y;
			record_position[2] = Ball[0].p.z;
			at[3] = record_position[0];
			at[4] = record_position[1];
			at[5] = record_position[2];
			Isrecroded = TRUE;
		}
		else
		{
			if (zoom < record_zoom + 100)
				zoom += 1;
			else
			{
				if (!IsMoving)
				{
					Sleep(1000);
					zoom = record_zoom;
					transPerc = FALSE;
					Isrecroded = FALSE;
				}
			}
		}
	}
	if (IsGameOver)
	{
		if (Ball[8].p.x == 100 || Ball[8].p.x == -100)
		{
			at[0] = 0;
			at[1] = 300;
			at[2] = -TABLE_IN_LENGTH;
			at[3] = 0;
			at[4] = TABLE_HEIGHT;
			at[5] = -TABLE_IN_LENGTH / 4;
		}
	}
	myDisplay();
}
// 球与球碰撞检测
void collideBalls(int j, int k)
{
	GLfloat dis, Cos, Sin, cCos, cSin, v1c, v1cc, v2c, v2cc;
	dis = sqrt(pow(Ball[k].p.x - Ball[j].p.x, 2) + pow(Ball[k].p.z - Ball[j].p.z, 2));
	if (dis<2 * Radius - 0.5)
	{
		mciSendString(TEXT("play audio//hit.wav"), NULL, 0, NULL);
		//PlaySound(TEXT("audio//hit.wav"), NULL, SND_FILENAME | SND_ASYNC);
		Cos = (Ball[k].p.x - Ball[j].p.x) / dis;
		Sin = (Ball[k].p.z - Ball[j].p.z) / dis;
		cCos = Sin*(-1);
		cSin = Cos;
		v1c = Ball[j].v.x*Cos + Ball[j].v.z*Sin;
		v1cc = Ball[j].v.x*cCos + Ball[j].v.z*cSin;
		v2c = Ball[k].v.x*Cos + Ball[k].v.z*Sin;
		v2cc = Ball[k].v.x*cCos + Ball[k].v.z*cSin;
		Ball[j].v.x = v1cc*cCos + v2c*Cos;
		Ball[j].v.z = v1cc*cSin + v2c*Sin;
		Ball[k].v.x = v1c*Cos + v2cc*cCos;
		Ball[k].v.z = v1c*Sin + v2cc*cSin;
		Ball[k].p.x = Ball[j].p.x + 2 * Radius*Cos;
		Ball[k].p.z = Ball[j].p.z + 2 * Radius*Sin;
	}
}
// 球与台边碰撞检测
void collideEdge(int j)
{
	if (fabs(Ball[j].p.x)>TABLE_IN_WIDTH / 2 - Radius)
	{
		if (Ball[j].p.x>0) Ball[j].p.x = TABLE_IN_WIDTH / 2 - Radius;
		if (Ball[j].p.x<0) Ball[j].p.x = -TABLE_IN_WIDTH / 2 + Radius;
		Ball[j].v.x *= (-1);
	}
	if (fabs(Ball[j].p.z)>TABLE_IN_LENGTH / 2 - Radius)
	{
		if (Ball[j].p.z>0) Ball[j].p.z = TABLE_IN_LENGTH / 2 - Radius;
		if (Ball[j].p.z<0) Ball[j].p.z = -TABLE_IN_LENGTH / 2 + Radius;
		Ball[j].v.z *= (-1);
	}
}
// 进球检测
int isBallIn(int j)
{
	GLfloat y_dis;
	y_dis = 2 * Radius *sin(PI / 3);
	if (sqrt(pow(Ball[j].p.x - (-TABLE_IN_WIDTH / 2 + POCKET_RADIUS), 2) + pow(Ball[j].p.z - (-TABLE_IN_LENGTH / 2 + POCKET_RADIUS), 2)) < Radius / 4 ||
		sqrt(pow(Ball[j].p.x - TABLE_IN_WIDTH / 2 + POCKET_RADIUS, 2) + pow(Ball[j].p.z - (-TABLE_IN_LENGTH / 2 + POCKET_RADIUS), 2)) < Radius / 4 ||
		sqrt(pow(Ball[j].p.x - (-TABLE_IN_WIDTH / 2 + POCKET_RADIUS), 2) + pow(Ball[j].p.z - TABLE_IN_LENGTH / 2 + POCKET_RADIUS, 2)) < Radius / 4 ||
		sqrt(pow(Ball[j].p.x - TABLE_IN_WIDTH / 2 + POCKET_RADIUS, 2) + pow(Ball[j].p.z - TABLE_IN_LENGTH / 2 + POCKET_RADIUS, 2)) < Radius / 4 ||
		sqrt(pow(Ball[j].p.x - (-TABLE_IN_WIDTH / 2 + POCKET_RADIUS), 2) + pow(Ball[j].p.z, 2)) < Radius / 4 ||
		sqrt(pow(Ball[j].p.x - -TABLE_IN_WIDTH / 2 + POCKET_RADIUS, 2) + pow(Ball[j].p.z, 2)) < Radius / 4)
	{
		//		Fired[j] = TRUE;
		mciSendString(TEXT("play audio//ballin.wav"), NULL, 0, NULL);
		if (j == 8)
		{
			IsGameOver = TRUE;
			mciSendString(TEXT("play audio//GameOver.wav"), NULL, 0, NULL);
		}
		if (j == 0)
		{
			Ball[0].p.x = 0;
			Ball[0].p.z = -TABLE_IN_LENGTH / 4;
			Ball[0].p.y = TABLE_HEIGHT + Radius;
			IsIllegal = 1;
		}
		//		else if (j == 8) { Ball[8].p.x = 0; Ball[8].p.y = 80; Ball[8].p.z = 70 + 2.0*y_dis; }
		else
		{
			Ball[j].isIn = 1;
			BallInNum += 1;
			Ball[j].p.z = -100 + BallInNum * 20;
			Ball[j].p.y = TABLE_HEIGHT - Radius;
			if (j > 8)
			{
				Ball[j].p.y = -100;
				if (IsFirstInBall)
				{
					PlayerBall[CurrPlayer] = 1;
					PlayerBall[1 - CurrPlayer] = 0;
					IsFirstInBall = FALSE;
					NextPlayer = CurrPlayer;
				}
				else if (PlayerBall[CurrPlayer] == 1) //花色球进了，且当前玩家是花色球玩家
				{
					NextPlayer = CurrPlayer;
				}
				if (PlayerBall[CurrPlayer] == 0) //犯规
					IsIllegal = 1;
			}
			else
			{
				Ball[j].p.y = -100;
				if (IsFirstInBall)
				{
					PlayerBall[CurrPlayer] = 0;
					PlayerBall[1 - CurrPlayer] = 1;
					IsFirstInBall = FALSE;
					NextPlayer = CurrPlayer;
				}
				else if (PlayerBall[CurrPlayer] == 0) //纯色球进了，且当前玩家是纯色球玩家
				{
					NextPlayer = CurrPlayer;
				}
				if (PlayerBall[CurrPlayer] == 1) //犯规
					IsIllegal = 1;
			}
		}
		Ball[j].v.x = 0;
		Ball[j].v.y = 0;
		Ball[j].v.z = 0;
		return 1;
	}
	return 0;
}
// 鼠标键盘反馈函数
static void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
	case 'd':
	case 'D':
		if (kx>-490) kx -= 10;
		break;
	case 'a':
	case 'A':
		if (kx<490) kx += 10;
		break;
	case 'w':
	case 'W':
		zoom -= 10;
		if (zoom<10) zoom = 10;
		break;
	case 's':
	case 'S':
		zoom += 10;
		if (zoom>500) zoom = 500;
		break;
	case '0':
		Fired[0] = !Fired[0];
		break;
	case '1':
		Fired[1] = !Fired[1];
		break;
	case '2':
		Fired[2] = !Fired[2];
		break;
	case '3':
		Fired[3] = !Fired[3];
		break;
	case '4':
		Fired[4] = !Fired[4];
		break;
	case '5':
		Fired[5] = !Fired[5];
		break;
	case '6':
		Fired[6] = !Fired[6];
		break;
	case '7':
		Fired[7] = !Fired[7];
		break;
	case '8':
		Fired[8] = !Fired[8];
		break;
	case '9':
		Fired[9] = !Fired[9];
		break;
	case 'p':
		Fired[10] = !Fired[10];
		break;
	case 'o':
		Fired[11] = !Fired[11];
		break;
	case 'i':
		Fired[12] = !Fired[12];
		break;
	case 'u':
		Fired[13] = !Fired[13];
		break;
	case 'y':
		Fired[14] = !Fired[14];
		break;
	case 't':
		Fired[15] = !Fired[15];
		break;
	case 'f':
		AllFired = !AllFired;

		//		break;
	}
}
static void myMouse(int mbutton, int mstate, int x, int y)
{
	if (IsMoving)
	{
		return;
	}
	at[3] = Ball[0].p.x;
	at[4] = Ball[0].p.y;
	at[5] = Ball[0].p.z;
	at[0] = zoom*(cos(anglex) * sin(angley)) + at[3];
	at[1] = zoom*(cos(angley)) + at[4];
	at[2] = zoom*(sin(anglex) * sin(angley)) + at[5];
	mx = x; my = y;
	if (mbutton == GLUT_RIGHT_BUTTON && mstate == GLUT_DOWN)
	{
		nowanglex = anglex; nowangley = angley; leftm = 0; rightm = 1;
		AimAt = 1;
	}
	else { rightm = 0; WaitHit = 0; }
	if (mbutton == GLUT_LEFT_BUTTON && mstate == GLUT_DOWN)
	{
		WaitHit = 1;
		NextPlayer = 1 - CurrPlayer;
		Hitted = FALSE;
		AimAt = 1;
	}
	if (mbutton == GLUT_LEFT_BUTTON && mstate == GLUT_UP)
	{
		Hit = 1;
		IsIllegal = 0;
		AimAt = 0;
		IsMoving = TRUE;
		transPerc = TRUE;
		updated = FALSE;
		Hitted = TRUE;
	}
	else Hit = 0;
}
static void mouseMove(int x, int y)
{
	at[3] = Ball[0].p.x;
	at[4] = Ball[0].p.y;
	at[5] = Ball[0].p.z;
	at[0] = zoom*(cos(anglex) * sin(angley)) + at[3];
	at[1] = zoom*(cos(angley)) + at[4];
	at[2] = zoom*(sin(anglex) * sin(angley)) + at[5];

	if (IsMoving)
		return;

	if (leftm) { kx = nowatx + x - mx; ky = nowaty + y - my; }
	if (rightm) { anglex = nowanglex + (x - mx)*0.01; angley = nowangley + (y - my)*0.01; }
	if (angley <= 0)
		angley = 0.1;
	if (angley > PI / 2)
		angley = PI / 2;
}
// 载入纹理
void initLoadTexture()
{
	Ball[1].texture = loadTexture("tex/B1.bmp");
	Ball[2].texture = loadTexture("tex/B2.bmp");
	Ball[3].texture = loadTexture("tex/B3.bmp");
	Ball[4].texture = loadTexture("tex/B4.bmp");
	Ball[5].texture = loadTexture("tex/B5.bmp");
	Ball[6].texture = loadTexture("tex/B6.bmp");
	Ball[7].texture = loadTexture("tex/B7.bmp");
	Ball[8].texture = loadTexture("tex/B8.bmp");
	Ball[9].texture = loadTexture("tex/B9.bmp");
	Ball[10].texture = loadTexture("tex/B10.bmp");
	Ball[11].texture = loadTexture("tex/B11.bmp");
	Ball[12].texture = loadTexture("tex/B12.bmp");
	Ball[13].texture = loadTexture("tex/B13.bmp");
	Ball[14].texture = loadTexture("tex/B14.bmp");
	Ball[15].texture = loadTexture("tex/B15.bmp");
	Ball[0].texture = loadTexture("tex/B16.bmp");
	texGround = loadTexture("tex/ground.bmp");//ground
	texWall = loadTexture("tex/wall.bmp");//wall
	texWall1 = loadTexture("tex/wall1.bmp");
	texWall2 = loadTexture("tex/wall2.bmp");
	tecCeiling = loadTexture("tex/ceiling.bmp");//天花板
	BZD = loadTexture("tex/black.bmp");
	texTableCloth = loadTexture("tex/green.bmp");//桌面
	texTable = loadTexture("tex/wood.bmp");//球桌边缘
	texCue = loadTexture("tex/wood.bmp");
	texgt = loadTexture("tex/green.bmp");
	texhe = loadTexture("tex/black.bmp");
	textureWard = loadTexture("tex/5.bmp");
	texturePaint1 = loadTexture("tex/6.bmp");
	texturePaint2 = loadTexture("tex/7.bmp");
	texture[2] = loadTexture("tex/flame2.bmp");
}
int isPowerOfTwo(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}
GLuint loadTexture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID, texture_ID = 0;
	FILE* pFile;
	fopen_s(&pFile, file_name, "rb");
	if (pFile == 0)
		return 0;
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0) ++line_bytes;
		total_bytes = line_bytes * height;
	}
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0) { fclose(pFile); return 0; }
	if (fread(pixels, total_bytes, 1, pFile) <= 0) { free(pixels); fclose(pFile); return 0; }
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!isPowerOfTwo(width) || !isPowerOfTwo(height) || width > max || height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0) { free(pixels); fclose(pFile); return 0; }
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0) { free(pixels); fclose(pFile); return 0; }
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (int*)&last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT,
		GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	free(pixels);
	return texture_ID;
}
// 光源
void initLight(void)
{
	//光照处理
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position0[] = { 0, 390, 0, 1 };
	//定义光位置得齐次坐标 (x,y,z,w), 如果w = 1.0, 为定位光源（也叫点光源） ，如果 w＝0，为定向光源（无限光源） ，定向光源为无穷远点，因而产生光为平行光
	GLfloat light_direction[] = { 0, -1, 0 };
	glLightfv(L0, GL_AMBIENT, light_ambient); // 环境光
	glLightfv(L0, GL_DIFFUSE, light_diffuse); // 漫射光
	glLightfv(L0, GL_SPECULAR, light_specular); // 镜面反射
	glLightfv(L0, GL_POSITION, light_position0); // 光照位置
	glLightfv(L0, GL_SPOT_DIRECTION, light_direction);//聚光方向
	glLightf(L0, GL_SPOT_CUTOFF, 180.0f);//聚光截至角
	glLightf(L0, GL_SPOT_EXPONENT, 0);//聚光指数
	glEnable(GL_LIGHTING); // 启动光照
	glEnable(L0); // 使第一盏灯有效
				  //材质处理
	GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 75.0 }; // 材质 RGBA 镜面指数，数值在 0～128 范围内
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_DEPTH_TEST); // 测试深度缓存
	glShadeModel(GL_SMOOTH);
}
void b_music()
{
	PlaySound(TEXT("audio//background.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	//mciSendString(TEXT("play audio//background.wav"), NULL, 0, NULL);
}

void drawString()
{
	char *str1 = "Current Player:  Player ";
	char str2[2];
	_itoa_s(CurrPlayer + 1, str2, 10);
	myString(18, 700, GLUT_BITMAP_TIMES_ROMAN_24, str1);
	myString(140, 700, GLUT_BITMAP_TIMES_ROMAN_24, str2);
}
void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT holdFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(holdFont);
}

void myString(float x, float y, void *font, char* c)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1024, 0, 768);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0);
	glBlendFunc(GL_ONE, GL_ZERO);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindTexture(GL_TEXTURE_2D, BZD);
	glRasterPos2f(x, y);
	for (c; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void updatePlayer()
{
	if (updated)
		return;
	if ((!transPerc) && Hitted)
	{
		if ((!IsIllegal) && (NextPlayer == CurrPlayer));
		else
			CurrPlayer = 1 - CurrPlayer;
		updated = TRUE;
	}
}