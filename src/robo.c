#include "robo.h"

// Variáveis para os ângulos dos diferentes membros do robô
GLfloat body = 0, shoulder1 = 0, shoulder10 = 0, shoulder2 = 0, shoulder20 = 0, elbow1 = 0, elbow2 = 0, leg1 = 0, leg1z = 0, knee1 = 0,
        leg2 = 0, leg2z = 0, knee2 = 0;

// Variáveis para o movimento e posição do robô
GLfloat moving, startx, starty, theta = 0.0;

// Ponto de vista da câmera (olho), ponto central e vetor 'up'
double eye[] = {0, 0, -1};
double center[] = {0, 0, 4};
double up[] = {0, 1, 0};

// Eixo perpendicular
double perpendicularAxis[] = {0, 0, 0};

// Distância do plano de recorte próximo
GLfloat Znear = 100.0;

// Largura e altura da janela
int windowWidth;
int windowHeight;

// Variáveis para os dedos do robô
int fingerBase1[5] = {0}, fingerUp1[5] = {0};
int fingerBase2[5] = {0}, fingerUp2[5] = {0};

// Ângulos adicionais para o robô
GLfloat angle_ = 0.0;  /* em graus */
GLfloat angle_2 = 0.0; /* em graus */

// Função para rotacionar um ponto em 3D em torno de um eixo arbitrário
void rotatePoint(double a[], double theta, double p[]) {
  double temp[3];

  // Armazena os valores originais do ponto p em temp
  temp[0] = p[0];
  temp[1] = p[1];
  temp[2] = p[2];

  // Calcula a componente ortogonal do ponto em relação ao eixo de rotação
  temp[0] = -a[2] * p[1] + a[1] * p[2];
  temp[1] = a[2] * p[0] - a[0] * p[2];
  temp[2] = -a[1] * p[0] + a[0] * p[1];

  // Multiplica a componente ortogonal pelo seno do ângulo de rotação
  temp[0] *= sin(theta);
  temp[1] *= sin(theta);
  temp[2] *= sin(theta);

  // Calcula a componente paralela do ponto em relação ao eixo de rotação e adiciona à componente ortogonal
  temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
  temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
  temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

  // Adiciona a componente original do ponto multiplicada pelo cosseno do ângulo de rotação
  temp[0] += cos(theta) * p[0];
  temp[1] += cos(theta) * p[1];
  temp[2] += cos(theta) * p[2];

  // Atualiza os valores do ponto p com as novas coordenadas rotacionadas
  p[0] = temp[0];
  p[1] = temp[1];
  p[2] = temp[2];
}

// Função para calcular o produto vetorial entre dois vetores
void crossProduct(double a[], double b[], double c[]) {
  // Cálculo do produto vetorial entre a e b armazenado em c
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}

// Função para normalizar um vetor
void normalize(double a[]) {
  double norm;

  // Calcula a norma do vetor
  norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
  norm = sqrt(norm);

  // Divide cada componente do vetor pela sua norma para obter o vetor normalizado
  a[0] /= norm;
  a[1] /= norm;
  a[2] /= norm;
}
// Inicializa as configurações do robô
void initRobo(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);  // Define a cor de fundo como preta
  glShadeModel(GL_FLAT);             // Define o modelo de sombreamento como plano
}

// Função para exibir os dedos do robô
void displayfingers(int armNumber) {
  float y = -0.3;  // Posição inicial em y dos dedos

  // Loop para percorrer todos os dedos
  for (int i = 0; i < 5; i++) {
    glPushMatrix();  // Salva a matriz atual

    // Translação e rotação do dedo base
    glTranslatef(armNumber ? 1.0 : -1.6, (i ? 0.3 : -0.2), (i ? (y + i * 0.18) : (y + i * 0.15)));
    glRotatef((GLfloat)fingerBase1[i], 0.0, 0.0, i ? -1.0 : 1.0);

    // Desenha o dedo base
    glTranslatef(0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.3, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    // Desenha a primeira falange do dedo
    glTranslatef(0.15, 0.0, 0.0);
    glRotatef((GLfloat)-fingerUp1[i], 0.0, 0.0, i ? -1.0 : -1.0);
    glTranslatef(0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.3, 0.1, 0.1);
    glutWireCube(1);
    glPopMatrix();

    glPopMatrix();  // Restaura a matriz salva anteriormente
  }
}

void displayRobo(void) {
  glClear(GL_COLOR_BUFFER_BIT);  // Limpa o buffer de cor da tela com a cor definida previamente (preta neste caso)

  glPushMatrix();  // Salva a matriz de modelagem atual na pilha

  gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1],
            up[2]);  // Define a posição da câmera, o ponto para o qual ela aponta e a direção "para cima"

  glScalef(0.7, 0.7, 0.7);  // Reduz a escala do objeto em 30% em relação à sua escala original

  glRotatef(angle_2, 1.0, 0.0, 0.0);  // Rotação do objeto em torno do eixo X com base no ângulo "angle_2"
  glRotatef(angle_, 0.0, 1.0, 0.0);   // Rotação do objeto em torno do eixo Y com base no ângulo "angle_"

  glTranslatef(0.0, 2.0, 1.0);  // Translação do objeto nos eixos X, Y e Z (neste caso, 2 unidades no eixo Y e 1 unidade no eixo Z)

  glPushMatrix();  // Salva a matriz de modelagem atual na pilha

  glScalef(2.0, 3.0, 1.0);  // Escala o objeto em 2 vezes no eixo X, 3 vezes no eixo Y e 1 vez no eixo Z

  glutWireCube(1.0);  // Desenha um cubo em formato de arame com lado de tamanho 1 na posição atual

  glPopMatrix();  // Restaura a matriz de modelagem anterior salva na pilha

  /*******************************************************
   *                        CABEÇA                       *
   ********************************************************/
  glPushMatrix(); /* PILHA DA CABEÇA */
  glTranslatef(0.0, 2.5, 0.0);
  glutSolidSphere(1.0, 50, 50); /* desenho da cabeça propriamente dita */
  glPopMatrix();                /* PILHA DA CABEÇA */

  /*******************************************************
   *                        OMBRO 1                       *
   ********************************************************/
  glPushMatrix(); /* PILHA DO BRAÇO 1 */
  glTranslatef(1.5, 1.2, 0.0);
  glRotatef((GLfloat)shoulder1, 0.0, 0.0, 1.0);
  glTranslatef(0.0, 0.0, 0.0);
  glRotatef((GLfloat)shoulder10, 0.0, 1.0, 0.0);
  glTranslatef(1.0, 0.0, 0.0);
  glPushMatrix(); /* PILHA DO OMBRO 1 */
  glScalef(2.0, 0.6, 1.0);
  glutWireCube(1.0); /* desenho do próprio ombro */
  glPopMatrix();     /* PILHA DO OMBRO 1 */

  /*******************************************************
   *                        COTOVELO 1                     *
   ********************************************************/
  glPushMatrix(); /* PILHA DO COTOVELO 1 */
  glTranslatef(1.0, 0.0, 0.0);
  glRotatef((GLfloat)elbow1, 0.0, 0.0, 1.0);
  glTranslatef(1.0, 0.0, 0.0);
  glPushMatrix(); /* PILHA DE DESENHO DO COTOVELO */
  glScalef(2.0, 0.6, 1.0);
  glutWireCube(1.0); /* desenho do próprio cotovelo */
  glPopMatrix();     /* PILHA DE DESENHO DO COTOVELO */

  /********************************************************
   *                        Dedos 1                     *
   ********************************************************/
  // Exibe os dedos do braço 1
  displayfingers(ARM1);
  glPopMatrix();  // PILHA DO COTOVELO 1
  glPopMatrix();  // PILHA DO BRAÇO 1
  /*******************************************************
   *                        OMBRO 2                   *
   ********************************************************/
  // Inicio do braço 2
  glPushMatrix();  // PILHA DO BRAÇO 2
  glTranslatef(-1.5, 1.2, 0.0);
  glRotatef((GLfloat)shoulder2, 0.0, 0.0, 1.0);
  glTranslatef(0.0, 0.0, 0.0);
  glRotatef((GLfloat)shoulder20, 0.0, 1.0, 0.0);
  glTranslatef(-1.0, 0.0, 0.0);

  // Ombro 2
  glPushMatrix();  // PILHA DO OMBRO 2
  glScalef(2.0, 0.6, 1.0);
  glutWireCube(1.0);  // desenho do próprio ombro
  glPopMatrix();      // PILHA DO OMBRO 2

  /*******************************************************
   *                        COTOVELO 2                      *
   ********************************************************/
  // Inicio do cotovelo 2
  glPushMatrix();  // PILHA DO COTOVELO 2
  glTranslatef(-1.0, 0.0, 0.0);
  glRotatef((GLfloat)elbow2, 0.0, 0.0, 1.0);
  glTranslatef(-1.0, 0.0, 0.0);

  // Desenho do cotovelo 2
  glPushMatrix();  // PILHA DE DESENHO DO COTOVELO 2
  glScalef(2.0, 0.6, 1.0);
  glutWireCube(1.0);  // desenho do próprio cotovelo
  glPopMatrix();      // PILHA DE DESENHO DO COTOVELO 2

  /********************************************************
   *                        DEDOS 2                     *
   ********************************************************/
  // Exibe os dedos do braço 2
  displayfingers(ARM2);
  glPopMatrix();  // PILHA DO COTOVELO 2
  glPopMatrix();  // PILHA DO BRAÇO 2

  /*******************************************************
   *                        PERNA 1                        *
   ********************************************************/
  // Inicio da perna 1
  glPushMatrix();  // PILHA DA PERN1A 1
  glTranslatef(1.0, -1.35, 0.0);
  glRotatef((GLfloat)leg1, 1.0, 0.0, 0.0);
  glRotatef((GLfloat)leg1z, 0.0, 0.0, 1.0);
  glTranslatef(-0.35, -1.2, 0.0);

  // Coxa 1
  glPushMatrix();  // PILHA DA COXA 1
  glScalef(0.7, 2.0, 1.0);
  glutWireCube(1.0);  // desenho da própria coxa
  glPopMatrix();      // PILHA DA COXA 1

  /*******************************************************
   *                        JOELHO 1                       *
   ********************************************************/
  // Inicio da parte inferior da perna 1
  glPushMatrix();  // PILHA DA PARTE INFERIOR DA PERN1A 1
  glTranslatef(1.0, -1.35, 0.0);
  glRotatef((GLfloat)knee1, 1.0, 0.0, 0.0);
  glTranslatef(-1, -0.7, 0.0);

  // Joelho 1
  glPushMatrix();  // PILHA DO JOELHO 1
  glScalef(0.7, 2.0, 1.0);
  glutWireCube(1.0);  // desenho do próprio joelho
  glPopMatrix();      // PILHA DO JOELHO 1

  // Tornozelo 1
  glTranslatef(0.0, -1.3, 0.0);
  glScalef(0.8, 0.7, 2.0);
  glutSolidCube(1.0);  // desenho do próprio tornozelo

  glPopMatrix();  // PILHA DA PARTE INFERIOR DA PERN1A 1
  glPopMatrix();  // PILHA DA PERN1A 1

  /*******************************************************
   *                        PERNA 2                        *
   ********************************************************/
  glPushMatrix();                            // Início da perna 2 (LEG 2 STACK)
  glTranslatef(1.0, -1.35, 0.0);             // Translação da perna 2
  glRotatef((GLfloat)leg2, 1.0, 0.0, 0.0);   // Rotação da perna 2 em torno do eixo X
  glTranslatef(-1.2, -0.0, 0.0);             // Translação após a rotação
  glRotatef((GLfloat)leg2z, 0.0, 0.0, 1.0);  // Rotação da perna 2 em torno do eixo Z
  glTranslatef(1.0, 0.0, 0.0);               // Translação após a rotação em Z
  glTranslatef(-1.53, -1.2, 0.0);            // Translação adicional da perna 2
  glPushMatrix();                            // Início da coxa 2 (THIGH 2 STACK)
  glScalef(0.7, 2.0, 1.0);                   // Escala da coxa 2
  glutWireCube(1.0);                         // Desenho da coxa 2
  glPopMatrix();                             // Fim da coxa 2 (THIGH 2 STACK)

  /*******************************************************
   *                        JOELHO 2                       *
   ********************************************************/
  glPushMatrix();                            // Início da parte inferior da perna 2 (LOWER LEG 2 STACK)
  glTranslatef(1.0, -1.35, 0.0);             // Translação da parte inferior da perna 2
  glRotatef((GLfloat)knee2, 1.0, 0.0, 0.0);  // Rotação do joelho 2 em torno do eixo X
  glTranslatef(-1, -0.7, 0.0);               // Translação após a rotação do joelho 2
  glPushMatrix();                            // Início do joelho 2 (KNEE 2 STACK)
  glScalef(0.7, 2.0, 1.0);                   // Escala do joelho 2
  glutWireCube(1.0);                         // Desenho do joelho 2
  glPopMatrix();                             // Fim do joelho 2 (KNEE 2 STACK)
  glTranslatef(0.0, -1.3, 0.0);              // Translação adicional da parte inferior da perna 2
  glScalef(0.8, 0.7, 2.0);                   // Escala do tornozelo 2
  glutSolidCube(1.0);                        // Desenho do tornozelo 2
  glPopMatrix();                             // Fim da parte inferior da perna 2 (LOWER LEG 2 STACK)
  glPopMatrix();                             // Fim da perna 2 (LEG 2 STACK)

  glPopMatrix();      // Fim do desenho (DRAWING STACK)
  glutSwapBuffers();  // Troca os buffers após criar o desenho
}

void reshape(int w, int h) {
  windowWidth = w;                                            // Atribui a largura da janela à variável windowWidth
  windowHeight = h;                                           // Atribui a altura da janela à variável windowHeight
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);                   // Define a janela de visualização com as dimensões da janela atual
  glMatrixMode(GL_PROJECTION);                                // Define a matriz de projeção como a matriz ativa
  glLoadIdentity();                                           // Inicializa a matriz de projeção com a matriz identidade
  printf("\n%f", Znear);                                      // Imprime o valor de Znear no console
  gluPerspective(Znear, (GLfloat)w / (GLfloat)h, 0.0, 20.0);  // Define a projeção perspectiva com os parâmetros especificados
  glMatrixMode(GL_MODELVIEW);                                 // Define a matriz de modelagem como a matriz ativa
  glLoadIdentity();                                           // Inicializa a matriz de modelagem com a matriz identidade
  glTranslatef(0.0, 0.0, -5.0);                               // Aplica uma translação na matriz de modelagem para mover a cena para trás
}

// control the movement of fingers and shoulder and elbow joints
void keyboardRobo(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
      // Movimento para frente
      crossProduct(up, center, perpendicularAxis);   // Calcula o produto vetorial entre "up" e "center" para obter um eixo perpendicular
      normalize(perpendicularAxis);                  // Normaliza o vetor perpendicularAxis
      rotatePoint(perpendicularAxis, 0.05, center);  // Rotaciona o vetor "center" em torno do eixo perpendicular no sentido horário
      rotatePoint(perpendicularAxis, 0.05, up);      // Rotaciona o vetor "up" em torno do eixo perpendicular no sentido horário
      glutPostRedisplay();                           // Solicita que a tela seja redesenhada
      break;

    case 's':
      // Movimento para trás
      crossProduct(up, center, perpendicularAxis);    // Calcula o produto vetorial entre "up" e "center" para obter um eixo perpendicular
      normalize(perpendicularAxis);                   // Normaliza o vetor perpendicularAxis
      rotatePoint(perpendicularAxis, -0.05, center);  // Rotaciona o vetor "center" em torno do eixo perpendicular no sentido anti-horário
      rotatePoint(perpendicularAxis, -0.05, up);      // Rotaciona o vetor "up" em torno do eixo perpendicular no sentido anti-horário
      glutPostRedisplay();                            // Solicita que a tela seja redesenhada
      break;

    case 'a':
      // Rotação para a esquerda
      rotatePoint(up, -0.05, center);  // Rotaciona o vetor "up" em torno do eixo "center" no sentido anti-horário
      glutPostRedisplay();             // Solicita que a tela seja redesenhada
      break;

    case 'd':
      // Rotação para a direita
      rotatePoint(up, 0.05, center);  // Rotaciona o vetor "up" em torno do eixo "center" no sentido horário
      glutPostRedisplay();            // Solicita que a tela seja redesenhada
      break;

    case 'q':
      // Diminui a distância focal da câmera (zoom in)
      if (Znear > 120) {
        Znear = 120;  // Limita o valor máximo de Znear
      } else if (Znear <= 60) {
        Znear = 60;  // Limita o valor mínimo de Znear
      } else {
        Znear -= 1.0;  // Diminui o valor de Znear
      }
      reshape(windowWidth, windowHeight);  // Atualiza a janela com a nova distância focal
      glutPostRedisplay();                 // Solicita que a tela seja redesenhada
      break;

    case 'Q':
      // Aumenta a distância focal da câmera (zoom out)
      if (Znear >= 120) {
        Znear = 120;  // Limita o valor máximo de Znear
      } else if (Znear < 60) {
        Znear = 60;  // Limita o valor mínimo de Znear
      } else {
        Znear += 1.0;  // Aumenta o valor de Znear
      }
      reshape(windowWidth, windowHeight);  // Atualiza a janela com a nova distância focal
      glutPostRedisplay();                 // Solicita que a tela seja redesenhada
      break;

    case '7':
      // Aumenta a rotação do ombro esquerdo do robot
      if (shoulder10 > 150) {
        shoulder10 = 150;  // Limita o valor máximo de rotação
      } else {
        shoulder10 = (shoulder10 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();            // Solicita que a tela seja redesenhada
      }
      break;

    case '8':
      // Diminui a rotação do ombro esquerdo do robot
      if (shoulder10 <= -90) {
        shoulder10 = -90;  // Limita o valor mínimo de rotação
      } else {
        shoulder10 = (shoulder10 - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();            // Solicita que a tela seja redesenhada
      }
      break;

    case 'p':
      // Aumenta a rotação do ombro direito do robot
      if (shoulder1 > 45) {
        shoulder1 = 45;  // Limita o valor máximo de rotação
      } else {
        shoulder1 = (shoulder1 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();          // Solicita que a tela seja redesenhada
      }
      break;

    case 'P':
      // Diminui a rotação do ombro direito do robot
      if (shoulder1 <= -90) {
        shoulder1 = -90;  // Limita o valor mínimo de rotação
      } else {
        shoulder1 = (shoulder1 - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();          // Solicita que a tela seja redesenhada
      }
      break;

    case 'o':
      // Aumenta a rotação do cotovelo esquerdo do robot
      if (shoulder2 <= -45) {
        shoulder2 = -45;  // Limita o valor máximo de rotação
      } else {
        shoulder2 = (shoulder2 - 5);  // Diminui a rotação em 5 graus (note que o sentido é invertido em relação ao ombro)
        glutPostRedisplay();          // Solicita que a tela seja redesenhada
      }
      break;

    case 'O':
      // Diminui a rotação do cotovelo esquerdo do robot
      if (shoulder2 > 90) {
        shoulder2 = 90;  // Limita o valor mínimo de rotação
      } else {
        shoulder2 = (shoulder2 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();          // Solicita que a tela seja redesenhada
      }
      break;

    case '9':
      // Aumenta a rotação do cotovelo direito do robot
      if (shoulder20 <= -150) {
        shoulder20 = -150;  // Limita o valor máximo de rotação
      } else {
        shoulder20 = (shoulder20 - 5);  // Diminui a rotação em 5 graus (note que o sentido é invertido em relação ao ombro)
        glutPostRedisplay();            // Solicita que a tela seja redesenhada
      }
      break;

    case '6':
      // Diminui a rotação do cotovelo direito do robot
      if (shoulder20 > 90) {
        shoulder20 = 90;  // Limita o valor mínimo de rotação
      } else {
        shoulder20 = (shoulder20 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();            // Solicita que a tela seja redesenhada
      }
      break;
      ////

    case 'i':
      // Aumenta a rotação do antebraço esquerdo do robot (elbow1)
      if (elbow1 >= 150) {
        elbow1 = 150;  // Limita o valor máximo de rotação
      } else {
        elbow1 = (elbow1 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();    // Solicita que a tela seja redesenhada
      }
      break;

    case 'I':
      // Diminui a rotação do antebraço esquerdo do robot (elbow1)
      if (elbow1 <= 0) {
        elbow1 = 0;  // Limita o valor mínimo de rotação
      } else {
        elbow1 = (elbow1 - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();    // Solicita que a tela seja redesenhada
      }
      break;

    case 'u':
      // Aumenta a rotação do antebraço direito do robot (elbow2)
      if (elbow2 >= 150) {
        elbow2 = 150;  // Limita o valor máximo de rotação
      } else {
        elbow2 = (elbow2 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();    // Solicita que a tela seja redesenhada
      }
      break;

    case 'U':
      // Diminui a rotação do antebraço direito do robot (elbow2)
      if (elbow2 <= 0) {
        elbow2 = 0;  // Limita o valor mínimo de rotação
      } else {
        elbow2 = (elbow2 - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();    // Solicita que a tela seja redesenhada
      }
      break;

    case 'y':
      // Aumenta a rotação da coxa esquerda do robot (leg1)
      if (leg1 >= 90) {
        leg1 = 90;  // Limita o valor máximo de rotação
      } else {
        leg1 = (leg1 + 5);    // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'Y':
      // Diminui a rotação da coxa esquerda do robot (leg1)
      if (leg1 <= -90) {
        leg1 = -90;  // Limita o valor mínimo de rotação
      } else {
        leg1 = (leg1 - 5);    // Diminui a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'z':
      // Aumenta a rotação da perna esquerda do robot em torno do eixo Z (leg1z)
      if (leg1z >= 90) {
        leg1z = 90;  // Limita o valor máximo de rotação
      } else {
        leg1z = (leg1z + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'Z':
      // Diminui a rotação da perna esquerda do robot em torno do eixo Z (leg1z)
      if (leg1z <= -90) {
        leg1z = -90;  // Limita o valor mínimo de rotação
      } else {
        leg1z = (leg1z - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;
      //
    case 't':
      // Aumenta a rotação da canela esquerda do robot (knee1)
      if (knee1 <= -90) {
        knee1 = -90;  // Limita o valor máximo de rotação
      } else {
        knee1 = (knee1 - 5);  // Diminui a rotação em 5 graus (note que o sentido é invertido em relação ao antebraço)
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'T':
      // Diminui a rotação da canela esquerda do robot (knee1)
      if (knee1 >= 0) {
        knee1 = 0;  // Limita o valor mínimo de rotação
      } else {
        knee1 = (knee1 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'r':
      // Aumenta a rotação da coxa direita do robot (leg2)
      if (leg2 >= 90) {
        leg2 = 90;  // Limita o valor máximo de rotação
      } else {
        leg2 = (leg2 + 5);    // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'R':
      // Diminui a rotação da coxa direita do robot (leg2)
      if (leg2 <= -90) {
        leg2 = -90;  // Limita o valor mínimo de rotação
      } else {
        leg2 = (leg2 - 5);    // Diminui a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case '2':
      // Aumenta a rotação da perna direita do robot em torno do eixo Z (leg2z)
      if (leg2z >= 90) {
        leg2z = 90;  // Limita o valor máximo de rotação
      } else {
        leg2z = (leg2z + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case '3':
      // Diminui a rotação da perna direita do robot em torno do eixo Z (leg2z)
      if (leg2z <= -90) {
        leg2z = -90;  // Limita o valor mínimo de rotação
      } else {
        leg2z = (leg2z - 5);  // Diminui a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;
      //
    case 'e':
      // Aumenta a rotação da canela direita do robot (knee2)
      if (knee2 <= -90) {
        knee2 = -90;  // Limita o valor máximo de rotação
      } else {
        knee2 = (knee2 - 5);  // Diminui a rotação em 5 graus (note que o sentido é invertido em relação ao antebraço)
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'E':
      // Diminui a rotação da canela direita do robot (knee2)
      if (knee2 >= 0) {
        knee2 = 0;  // Limita o valor mínimo de rotação
      } else {
        knee2 = (knee2 + 5);  // Aumenta a rotação em 5 graus
        glutPostRedisplay();  // Solicita que a tela seja redesenhada
      }
      break;

    case 'l':
      // Aumenta a rotação da base do dedo 1 (fingerBase1[0])
      if (fingerBase1[0] < 90) {
        fingerBase1[0] = (fingerBase1[0] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'L':
      // Diminui a rotação da base do dedo 1 (fingerBase1[0])
      if (fingerBase1[0] > 0) {
        fingerBase1[0] = (fingerBase1[0] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'k':
      // Aumenta a rotação do segmento superior do dedo 1 (fingerUp1[0])
      if (fingerUp1[0] < 90) {
        fingerUp1[0] = (fingerUp1[0] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'K':
      // Diminui a rotação do segmento superior do dedo 1 (fingerUp1[0])
      if (fingerUp1[0] > 0) {
        fingerUp1[0] = (fingerUp1[0] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'j':
      // Aumenta a rotação da base do dedo 2 (fingerBase1[1])
      if (fingerBase1[1] < 90) {
        fingerBase1[1] = (fingerBase1[1] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'J':
      // Diminui a rotação da base do dedo 2 (fingerBase1[1])
      if (fingerBase1[1] > 0) {
        fingerBase1[1] = (fingerBase1[1] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'h':
      // Aumenta a rotação do segmento superior do dedo 2 (fingerUp1[1])
      if (fingerUp1[1] < 90) {
        fingerUp1[1] = (fingerUp1[1] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;
    case 'H':
      // Diminui a rotação do segmento superior do dedo 3 (fingerUp1[1])
      if (fingerUp1[1] > 0) {
        fingerUp1[1] = (fingerUp1[1] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'g':
      // Aumenta a rotação da base do dedo 4 (fingerBase1[2])
      if (fingerBase1[2] < 90) {
        fingerBase1[2] = (fingerBase1[2] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'G':
      // Diminui a rotação da base do dedo 4 (fingerBase1[2])
      if (fingerBase1[2] > 0) {
        fingerBase1[2] = (fingerBase1[2] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'f':
      // Aumenta a rotação do segmento superior do dedo 4 (fingerUp1[2])
      if (fingerUp1[2] < 90) {
        fingerUp1[2] = (fingerUp1[2] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;

    case 'F':
      // Diminui a rotação do segmento superior do dedo 4 (fingerUp1[2])
      if (fingerUp1[2] > 0) {
        fingerUp1[2] = (fingerUp1[2] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      }
      glutPostRedisplay();  // Solicita que a tela seja redesenhada
      break;
    case 'm':
      if (fingerBase1[3] < 90) fingerBase1[3] = (fingerBase1[3] + 5) % 360;
      glutPostRedisplay();
      break;
    case 'M':
      if (fingerBase1[3] > 0) fingerBase1[3] = (fingerBase1[3] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                                  // Solicita que a tela seja redesenhada
      break;
    case 'n':
      if (fingerUp1[3] < 90) fingerUp1[3] = (fingerUp1[3] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                             // Solicita que a tela seja redesenhada
      break;
    case 'N':
      if (fingerUp1[3] > 0) fingerUp1[3] = (fingerUp1[3] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                            // Solicita que a tela seja redesenhada
      break;
    case 'b':
      if (fingerBase1[4] < 90) fingerBase1[4] = (fingerBase1[4] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                                   // Solicita que a tela seja redesenhada
      break;
    case 'B':
      if (fingerBase1[4] > 0) fingerBase1[4] = (fingerBase1[4] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                                  // Solicita que a tela seja redesenhada
      break;
    case 'v':
      if (fingerUp1[4] < 90) fingerUp1[4] = (fingerUp1[4] + 5) % 360;  // Aumenta a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                             // Solicita que a tela seja redesenhada
      break;
    case 'V':
      if (fingerUp1[4] > 0) fingerUp1[4] = (fingerUp1[4] - 5) % 360;  // Diminui a rotação em 5 graus e mantém o valor entre 0 e 360
      glutPostRedisplay();                                            // Solicita que a tela seja redesenhada
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

void mouseRobo(int button, int state, int x, int y) {
  // Função de callback para tratamento de eventos de clique do mouse

  if (button == GLUT_LEFT_BUTTON) {
    // Verifica se o botão esquerdo do mouse foi clicado

    if (state == GLUT_DOWN) {
      // Verifica se o botão do mouse foi pressionado
      moving = 1;  // Define a variável moving como verdadeira, indicando que o usuário está movendo o mouse
      startx = x;  // Armazena a posição inicial do mouse no eixo x
      starty = y;  // Armazena a posição inicial do mouse no eixo y
    }
    if (state == GLUT_UP) {
      // Verifica se o botão do mouse foi solto
      moving = 0;  // Define a variável moving como falsa, indicando que o usuário parou de mover o mouse
    }
  }
}

void motion(int x, int y) {
  // Função de callback para tratamento de movimento do mouse

  if (moving) {
    // Verifica se a variável moving é verdadeira, indicando que o usuário está movendo o mouse

    int dx = x - startx;  // Calcula a diferença entre a posição atual do mouse e a posição inicial no eixo x
    int dy = y - starty;  // Calcula a diferença entre a posição atual do mouse e a posição inicial no eixo y

    rotatePoint(up, -(dx % 360) * 0.01,
                center);  // Rotaciona o vetor "up" em torno do eixo central com base no movimento do mouse no eixo x

    crossProduct(up, center, perpendicularAxis);  // Calcula o produto vetorial entre "up" e "center" para obter um eixo perpendicular
    normalize(perpendicularAxis);                 // Normaliza o vetor perpendicularAxis

    rotatePoint(perpendicularAxis, -dy * 0.01,
                center);  // Rotaciona o vetor "center" em torno do eixo perpendicular com base no movimento do mouse no eixo y
    rotatePoint(perpendicularAxis, -dy * 0.01,
                up);  // Rotaciona o vetor "up" em torno do eixo perpendicular com base no movimento do mouse no eixo y

    startx = x;  // Atualiza a posição inicial do mouse no eixo x
    starty = y;  // Atualiza a posição inicial do mouse no eixo y

    glutPostRedisplay();  // Solicita que a tela seja redesenhada
  }
}

void abrirRobo() {
  // Função para inicializar e abrir a janela do robot

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  // Define o modo de exibição da janela como duplo buffer e cor RGB
  glutInitWindowSize(5000, 5000);               // Define o tamanho da janela como 5000x5000 pixels
  glutInitWindowPosition(10, 10);               // Define a posição inicial da janela na tela como (10,10)
  glutCreateWindow("Robo");                     // Cria a janela com o título "Robo"

  glutMouseFunc(mouseRobo);        // Registra a função de callback para tratamento de eventos de clique do mouse
  glutMotionFunc(motion);          // Registra a função de callback para tratamento de movimento do mouse
  glutDisplayFunc(displayRobo);    // Registra a função de callback para desenho da cena
  glutReshapeFunc(reshape);        // Registra a função de callback para redimensionamento da janela
  glutKeyboardFunc(keyboardRobo);  // Registra a função de callback para tratamento de eventos de teclado

  initRobo();  // Chama a função de inicialização do robot

  glutMainLoop();  // Inicia o loop principal de eventos da GLUT
}
