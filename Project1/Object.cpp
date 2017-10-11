#define _CRT_SECURE_NO_WARNINGS
#include "Object.h"
#include<iostream>
#include<cstdio>

using namespace std;

 Object::Object(){
	 pos = { 0.0f,0.0f,0.0f };
	 velocity = { 0.0f,0.0f,0.0f };
	 isBall = 1;
}

 void Object::reset() {
	 pos = { 0.0f,0.0f,0.0f };
	 velocity = { 0.0f,0.0f,0.0f };
 }

 void Object::toBall() {
	 isBall = 1;
 }
 
 void Object::jump() {
	 velocity[1] += JUMP_STRENGTH;
 }

 int Object::status() {
	 return isBall;
 }

 void Object::load(char* filename) {
	 V.clear();
	 F.clear();
	 isBall = 0;
	 char line[100];
	 char mark[5];
	 float x, y, z;
	 char *pch;
	 vector<int> faceIndex;
	 int index;

	 FILE* fp = fopen(filename, "r");
	 V.push_back(Point(0, 0, 0));

	 if (!fp) {
		 cout << "Couldn't open the file!" << endl;
		 exit(1);
	 }

	 while (fgets(line, 100, fp)) {
		 switch (line[0]) {
		 case 'v':
			 sscanf(line, "%s %f %f %f", mark, &x, &y, &z);
			 V.push_back(Point(x, y, z));
			 break;
		 case 'f':
			 pch = strtok(line, " ");
			 pch = strtok(NULL, " ");
			 while (pch != NULL)
			 {
				 sscanf(pch, "%d", &index);
				 faceIndex.push_back(index);
				 pch = strtok(NULL, " ");
			 }
			 F.push_back(faceIndex);
			 faceIndex.clear();
			 break;
		 default:
			 break;
		 }
	 }
 }

 void Object::draw(float y) {
	 if (isBall) {
		 glPushMatrix();
		 glTranslatef(0, y + 0.5, 0); //take r into consideration
		 glutSolidSphere(0.5, 500, 500);
		 glPopMatrix();
	 }
	 else {
		 glPushMatrix();
		 glTranslatef(0, y, 0);
		 glColor3d(0, 1, 0);
		 glBegin(GL_TRIANGLES);
		 /*for (int i = 0, num = F.size(); i<num; ++i) {
			 for (int j = 0, n = F[i].size(); j<n; ++j) {
				 glVertex3dv(V[F[i][j]].data());
				 glVertex3dv(V[F[i][(j + 1) % n]].data());
			 }
		 }*/
		 for (int i = 0, num = F.size(); i < num; i++) {
			 glVertex3dv(V[F[i][0] ].data());
			 glVertex3dv(V[F[i][1] ].data());
			 glVertex3dv(V[F[i][2] ].data());
		 }
		 glEnd();
		 glPopMatrix();
	 }
 }