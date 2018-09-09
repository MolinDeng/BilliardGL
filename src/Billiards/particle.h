#pragma once  
#define FREEGLUT_STATIC
#define GLUT_DISABLE_ATEXIT_HACK  
#include "vec.h"
#include <gl/glut.h>
#include <iostream>
using namespace std;


class particle {
	bool has_tex;//纹理或颜色 

	vec place;//位置
	vec size;//大小
	vec speed;// 速度
	vec acc;//加速度 
	vec angle;//角度
	vec color;//颜色

	unsigned int texture;//纹理  
	float life;//生命 
	friend class emitter;

	void draw(GLuint texture);//绘制粒子（纹理）  
	void draw();//绘制粒子（颜色）
	void show();
	void update();

public:
	const vec& getPlace();
	const vec& getSize();
	const vec& getAngle();
	particle(vec size, vec speed, vec acc, vec angle,
		float _life, unsigned int _texture);

	particle(vec size, vec speed, vec acc, vec angle,
		float _life, vec _color);
};

class emitter {
	float x1, y1, x2, y2, z1, z2; //发射器位置  
	int speed;//发射速率  
	bool isforever;
	int num;
	particle **p;//发射粒子  
	bool *dead;
	particle* (*f)();
	//初始化粒子的函数指针  
	bool(*isDead)(particle*);

public:
	void emit(float _x1,
		float _x2, float _y1, float _y2, float _z1, float _z2);
	void update();
	void show();
	emitter(particle* (init)(), int _speed, float _x1,
		float _x2, float _y1, float _y2, float _z1, float _z2);
	~emitter();
};