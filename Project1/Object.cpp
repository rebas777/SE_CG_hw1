#define _CRT_SECURE_NO_WARNINGS
#include "Object.h"
#include<iostream>
#include<cstdio>


#define Mag(Normal) (sqrt(Normal[0]*Normal[0] + Normal[1]*Normal[1] + Normal[2]*Normal[2]))

using namespace std;


static char* objName1 = "suzanne.obj";
static char* objName2 = "body.obj";
static char* objName3 = "star.obj";

 Object::Object(){
	 pos = { 0.0f,0.0f,0.0f };
	 velocity = { 0.0f,0.0f,0.0f };
	 isBall = 1;
	 load(objName1);
	 toBall();
}

 void Object::reset() {
	 pos = { 0.0f,0.0f,0.0f };
	 velocity = { 0.0f,0.0f,0.0f };
 }

 void Object::toBall() {
	 isBall = 1;
 }

 void Object::toObj() {
	 isBall = 0;
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

	 computeNormals();
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
		 //遍历每个面
		 for (int i = 0, num = F.size(); i < num; i++) {
			 glNormal3dv(N[F[i][0] ].data());
			 glVertex3dv(V[F[i][0] ].data());
			 glNormal3dv(N[F[i][1]].data());
			 glVertex3dv(V[F[i][1] ].data());
			 glNormal3dv(N[F[i][2]].data());
			 glVertex3dv(V[F[i][2] ].data());
		 }
		 glEnd();
		 glPopMatrix();
	 }
 }

 Vector3d Object::Vector(Vector3d vPoint1, Vector3d vPoint2) {
	 Vector3d vVector;
	 vVector[0] = vPoint1[0] - vPoint2[0];
	 vVector[1] = vPoint1[1] - vPoint2[1];
	 vVector[2] = vPoint1[2] - vPoint2[2];
	 return vVector;
 }

 Vector3d Object::AddVector(Vector3d vVector1, Vector3d vVector2) {
	 Vector3d vResult;
	 vResult[0] = vVector2[0] + vVector1[0];
	 vResult[1] = vVector2[1] + vVector1[1];
	 vResult[2] = vVector2[2] + vVector1[2];
	 return vResult;
 }

 Vector3d Object::DivideVectorByScaler(Vector3d vVector1, float Scaler) {
	 Vector3d vResult;
	 vResult[0] = vVector1[0] / Scaler;
	 vResult[1] = vVector1[1] / Scaler;
	 vResult[2] = vVector1[2] / Scaler;
	 return vResult;
 }

 Vector3d Object::Cross(Vector3d vVector1, Vector3d vVector2) {
	 Vector3d vCross;
	 vCross[0] = ((vVector1[1] * vVector2[2]) - (vVector1[2] * vVector2[1]));
	 vCross[1] = ((vVector1[2] * vVector2[0]) - (vVector1[0] * vVector2[2]));
	 vCross[2] = ((vVector1[0] * vVector2[1]) - (vVector1[1] * vVector2[0]));
	 return vCross;
 }

 Vector3d Object::Normalize(Vector3d vNormal) {
	 double Magnitude;
	 Magnitude = Mag(vNormal);      // 获得矢量的长度 
	 vNormal[0] /= (float)Magnitude;
	 vNormal[1] /= (float)Magnitude;
	 vNormal[2] /= (float)Magnitude;
	 return vNormal;
 }

 void Object::computeNormals() {
	 Vector3d vVector1, vVector2, vNormal, vPoly[3];

	 Vector3d *pNormals = new Vector3d[F.size()];
	 Vector3d *pTempNormals = new Vector3d[F.size()];
	 
	 //遍历obj所有的面
	 for (int i = 0; i < F.size(); i++) {
		 vPoly[0] = V[F[i][0]];
		 vPoly[1] = V[F[i][1]];
		 vPoly[2] = V[F[i][2]];

		 //计算该面的法向量
		 vVector1 = Vector(vPoly[0], vPoly[2]);   // 获得多边形的矢量 
		 vVector2 = Vector(vPoly[2], vPoly[1]);   // 获得多边形的第二个矢量 

		 vNormal = Cross(vVector1, vVector2);   // 获得两个矢量的叉积 
		 pTempNormals[i] = vNormal;      // 保存非规范化法向量 
		 vNormal = Normalize(vNormal);     // 规范化获得的叉积 
		 pNormals[i] = vNormal;
	 }


		 Vector3d vSum = { 0.0, 0.0, 0.0 };
		 Vector3d vZero = vSum;
		 int shared = 0;
		 //遍历所有的顶点
		 for (int i = 0; i < V.size(); i++) {
			 //遍历所有的面
			 for (int j = 0; j < F.size(); j++) {
			     //判断点是否在面上
				 if (F[j][0] == i || F[j][1] == i || F[j][2] == i) {
					 vSum = AddVector(vSum, pTempNormals[j]);
					 shared++;
				 }
			 }
			 N.push_back(Normalize(DivideVectorByScaler(vSum, float(-shared))));

			 vSum = vZero;
			 shared = 0;
		 }
 }