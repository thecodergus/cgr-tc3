#include <GL/glut.h>
#include <stdio.h>

#include "robo.h"

int main(int argc, char** argv) {
  glutInit(&argc, argv);  // inicializa a biblioteca GLUT
  abrirRobo();
  return 0;  // retorna 0 indicando que o programa foi finalizado com sucesso
}
