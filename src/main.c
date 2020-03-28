#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#define WINDOW_TITLE "Exercice 1"

void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void renderFunction(void);
void keyboard(unsigned char, int, int);
void initData(char*);
double interpolatePos(double, double, double, double, double);


int currentWidth = 800;
int currentHeight = 600;
int windowHandle = 0;

//

typedef struct _Point3{
	double x;
	double y;
	double z;
	double val;
} Point3;

typedef struct _Cube{
	Point3 corner[8];
} Cube;

void interpolatePos3(Point3 *, Point3, Point3, double);
void drawCubeFrom(Point3, double);
//

double display_size = 0.4;
int display_number = 0;
int display_mode = 0;
#define MAX_MODE 4

GLfloat angle_h = 0.0;
GLfloat angle_v = 0.0;
int prev_x = -1;
int prev_y = -1;

Point3 * dataArray;
double iso = 2.5;
int size = 0;

int main(int argc,char* argv[]){
	initialize(argc, argv);

	initData("src/datas");

	glutMainLoop();

	if (dataArray){
		free(dataArray);
	}

	exit(EXIT_SUCCESS);
}

void initData(char* path){
	FILE * fp = fopen(path, "r");
	char ch[20];
	fgets(ch, sizeof(ch), fp);
	size = atoi(ch);
	int nb_value = size*size*size;
	
	dataArray = malloc(sizeof(Point3) * nb_value);

	int i = 0;
	while(fgets(ch, sizeof(ch), fp) && i < nb_value){
		dataArray[i].val = atof(ch);
		i++;
	}
	double step = (display_size*2) / (size-1);
	for (int i = 0 ; i < size ; i++){
		for (int j = 0 ; j < size ; j++){
			for (int k = 0 ; k < size ; k++){
				int index = i*size*size+j*size+k ;
				dataArray[index].x = step * i - display_size;
				dataArray[index].y = step * j - display_size;
				dataArray[index].z = step * k - display_size;
 			}
		}
	}
}

void renderFunction(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glRotatef(angle_h, 0, 1, 0);
	glRotatef(angle_v, 1, 0, 0);
	
	// Label iso
	char ch[20];
	sprintf(ch, "%.3f", iso);
	glRasterPos3f(display_size+0.15, display_size+0.15,display_size+0.15);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, ch);

	// each value has a dot and text
	
	for(int i = 0 ; i < size*size*size; i++){
		if (display_mode > 0){
			glBegin(GL_POINTS);
			glVertex3f(dataArray[i].x, dataArray[i].y, dataArray[i].z);
			glEnd();
		}
		if (display_number){
			glRasterPos3f(dataArray[i].x, dataArray[i].y, dataArray[i].z);
			sprintf(ch, "%.2f", dataArray[i].val);
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, ch);
		}
	}
	

	// Cubes extérieur (x y z)
	if (display_mode > 1){
		Point3 po;
		po.x = display_size;
		po.y = display_size;
		po.z = display_size;
		drawCubeFrom(po, display_size*2);
	}
	//
	double step = (display_size*2) / (size-1);
	if (display_mode > 2){
		for (int i = 1 ; i < size ; i++){
			for (int j = 1 ; j < size ; j++){
				for (int k = 1 ; k < size ; k++){
					int index = i*size*size+j*size+k ;
					drawCubeFrom(dataArray[index], step);
	 			}
			}
		}
	}

	

	

	for (int i = 0; i < size-1 ;i ++){
		for (int j = 0; j < size-1 ; j++){
			for (int k = 0; k < size-1 ; k++){
				int c = 1;
				switch (c) {
					case 1:
						//nothing todo
						break;
					default:
						fprintf(stderr, "error -> i:%d, j:%d, k:%d \n",i,j,k);
				}
			}
		}
	}
	

	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	switch(key){
		case '4':
			angle_h++;
			break;
		case '6':
			angle_h--;
			break;
		case '5':
			display_number = (display_number+1)%2;
			break;
		case '+':
			iso += 0.005;
			break;
		case '-':
			iso -= 0.005;
			break;
		case 32: // espace
			display_mode = (display_mode+1)%MAX_MODE;
			break;
		case 27: // Esc
			exit(1);
		//default:
			//printf("%d\n",key);
	}
	glutPostRedisplay();
}

void passiveMotion(int x, int y){
	prev_x = x;
	prev_y = y;
}

void motion(int x, int y){
	if (prev_x!=-1){
		angle_h += x-prev_x;
		glutPostRedisplay();
	}

	if (prev_y!=-1){
		angle_v += y-prev_y;
		glutPostRedisplay();
	}
	prev_x = x;
	prev_y = y;
}

void initialize(int argc, char* argv[]){
	initWindow(argc, argv);
	fprintf(stdout, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	
	/*routine permettant de définir la couleur de vidage qui prend comme paramètre les composants
     rouge, vert, bleu de la couleur plus la valeur alpha.*/
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
}

void initWindow(int argc, char* argv[]){
	glutInit(&argc, argv);
	/*
	   glutInitContextVersion(4, 0);
	   glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	   glutInitContextProfile(GLUT_CORE_PROFILE);

	   glutSetOption(
	   GLUT_ACTION_ON_WINDOW_CLOSE,
	   GLUT_ACTION_GLUTMAINLOOP_RETURNS
	   );
	   */
	glutInitWindowSize(currentWidth, currentHeight);


	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	windowHandle = glutCreateWindow(WINDOW_TITLE);

	if(windowHandle < 1) {
		fprintf(stderr, "ERROR: Could not create a new rendering window.\n");
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(resizeFunction);
	glutDisplayFunc(renderFunction);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
}

void resizeFunction(int width, int height){
	currentWidth = width;
	currentHeight = height;
	glViewport(0, 0, currentWidth, currentHeight);
}

// math

double interpolatePos(double c1, double c2, double iso, double v1, double v2){
	return c1 + ((iso-v1)/(v2-v1)) * (c2-c1);
}

void interpolatePos3(Point3 *p, Point3 p1, Point3 p2, double iso){
	p->x = p1.x + ((iso-p1.val)/(p2.val-p1.val)) * (p2.x-p1.x);
	p->y = p1.y + ((iso-p1.val)/(p2.val-p1.val)) * (p2.y-p1.y);
	p->z = p1.z + ((iso-p1.val)/(p2.val-p1.val)) * (p2.z-p1.z);
}

// drawing functions

void drawCubeFrom(Point3 p, double size){
	glBegin(GL_LINE_LOOP);  //
	p.x = -p.x;
	//face avant
	glVertex3f(p.x, p.y, p.z);
	glVertex3f(p.x+size, p.y, p.z); 
	glVertex3f(p.x+size, p.y-size, p.z);
	glVertex3f(p.x, p.y-size, p.z);
	glEnd();
	//face arriére
	glBegin(GL_LINE_LOOP);
	glVertex3f(p.x, p.y, p.z-size);
	glVertex3f(p.x+size, p.y, p.z-size); 
	glVertex3f(p.x+size, p.y-size, p.z-size);
	glVertex3f(p.x, p.y-size, p.z-size);
	glEnd();
	//fermeture du cube
	glBegin(GL_LINES);  // x y z
	glVertex3f( p.x, p.y, p.z);
	glVertex3f( p.x, p.y, p.z-size);
	//
	glVertex3f( p.x+size, p.y, p.z);
	glVertex3f( p.x+size, p.y, p.z-size);
	//
	glVertex3f( p.x, p.y-size, p.z);
	glVertex3f( p.x, p.y-size, p.z-size);
	//
	glVertex3f( p.x+size, p.y-size, p.z);
	glVertex3f( p.x+size, p.y-size, p.z-size);
	glEnd();
}


