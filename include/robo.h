#ifndef B085289E_0798_42E4_9349_095F2E750FEA
#define B085289E_0798_42E4_9349_095F2E750FEA

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>

#define ARM1 1
#define ARM2 0

void rotatePoint(double a[], double theta, double p[]);
void crossProduct(double a[], double b[], double c[]);
void normalize(double a[]);
void initRobo(void);
void displayfingers(int armNumber);
void displayRobo(void);
void reshape(int w, int h);
void keyboardRobo(unsigned char key, int x, int y);
void mouseRobo(int button, int state, int x, int y);
void motion(int x, int y);
void abrirRobo();

#endif /* B085289E_0798_42E4_9349_095F2E750FEA */
