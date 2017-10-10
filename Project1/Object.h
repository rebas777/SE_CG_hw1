#pragma once
#include<iostream>
#include<GL/glut.h>
#include<string>
#include<vector>
#include<Eigen/Core>

#define JUMP_STRENGTH 1.0; 
#define RADIUS 0.5;

using namespace std;
using namespace Eigen;

typedef Vector3d Point;
typedef Vector3d Pos;
typedef Vector3d Velocity;

class Object {
public:
	Object();
	void load(char* filename);
	void draw(float y);
	void reset();
	void jump();
	void toBall();
	int status();
	Velocity velocity;
	Pos pos;
	

private:
	int isBall;
	vector<Point> V;
	vector<vector<int> > F;
};
