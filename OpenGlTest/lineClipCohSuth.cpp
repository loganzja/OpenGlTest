#include <GL/glut.h>
#include <stdio.h>

GLint winWidth = 500, winHeight = 500;
bool mouseClickedFlag = FALSE;
bool drawedFlag = FALSE;
GLint x1, y1, x2, y2;
GLint endPoints[200][2];
int index = 0;

void init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
}



void drawLine(GLint x1, GLint y1, GLint x2, GLint y2)
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2i(x1, y1);
		glVertex2i(x2, y2);
	glEnd();
	glFlush();
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.25*winWidth, 0.25*winHeight);
	glVertex2f(0.75*winWidth, 0.25*winHeight);
	glVertex2f(0.75*winWidth, 0.75*winHeight);
	glVertex2f(0.25*winWidth, 0.75*winHeight);
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (int i = 0; i < index; i += 2)
	{
		//printf("yes\n");
		glVertex2i(endPoints[i][0], endPoints[i][1]);
		glVertex2i(endPoints[i + 1][0], endPoints[i + 1][1]);	
	}
	glEnd();
	glFlush();
}

void mouseClickFunc(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
	{
		mouseClickedFlag == TRUE ? mouseClickedFlag = FALSE : mouseClickedFlag = TRUE;
		if (mouseClickedFlag ==TRUE)
		{
			x1 = xMouse;
			y1 = winHeight - yMouse;
		}
		if (drawedFlag == TRUE)
		{
			drawedFlag = FALSE;
			endPoints[index][0] = x1;
			endPoints[index][1] = y1;
			index++;
			endPoints[index][0] = x2;
			endPoints[index][1] = y2;
			index++;
		}
	}
}

void mouseMoveFunc(GLint xMouse, GLint yMouse)
{
	if (mouseClickedFlag == TRUE)
	{
		x2 = xMouse;
		y2 = winHeight - yMouse;
		
		glClear(GL_COLOR_BUFFER_BIT);
		myDisplay();
		drawLine(x1, y1, x2, y2);
		drawedFlag = TRUE;
	}
}



void reshapeFcn(GLint newWidth, GLint newHeight) {
	winWidth = newWidth;
	winHeight = newHeight;
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);	//视图窗口大小
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (GLdouble)newWidth, 0, (GLdouble)newHeight);

	//glClear(GL_COLOR_BUFFER_BIT);

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("裁剪算法");
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshapeFcn);

	glutMouseFunc(mouseClickFunc);
	glutPassiveMotionFunc(mouseMoveFunc);

	glutMainLoop();
}