#include <GL/glut.h>
#include <stdio.h>

GLint winWidth = 500, winHeight = 500;
bool mouseClickedFlag = FALSE;
bool drawedFlag = FALSE;
GLint x1, y1, x2, y2;
GLint endPoints[200][2];
int index = 0;

class wcPt2D {
public:
	GLfloat x, y;
};

wcPt2D winMin, winMax, p1, p2;

void init() {
	winMin.x = 125;
	winMin.y = 125;
	winMax.x = 375;
	winMax.y = 375;
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

inline void swap(GLint& a, GLint& b) { GLint t = a; a = b; b = t; }

inline void setPixel(GLint x, GLint y)
{
	if (x < 125 || x>375 || y < 125 || y>375) {
		glColor3f(0.0, 1.0, 0.0);
	}
	else
		glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void Bresenham(GLint x1, GLint y1, GLint x2, GLint y2)
{
	if (x2 < x1)
	{
		swap(x2, x1);
		swap(y2, y1);
	}
	//画第一个点
	int x, y;
	x = x1;
	y = y1;
	setPixel(x, y);
	//首先处理直线平行坐标轴
	if (y1 == y2)
	{
		//平行x轴
		while (x < x2)
		{
			x++;
			setPixel(x, y);
		}
		return;
	}
	if (x1 == x2)
	{
		//平行y轴
		while (y < y2)
		{
			y++;
			setPixel(x, y);
		}
		return;
	}
	int dx = x2 - x1, dy = y2 - y1;
	int p;
	int twoDy = 2 * dy, twoMinusDx = 2 * (dy - dx), twoDx = 2 * dx, twoMinusDy = 2 * (dx - dy);
	int twoSum = 2 * (dy + dx);
	double k = (double)dy / (double)dx;

	//0<k<1的情况
	if (k<1.0&&k>0.0)
	{
		p = 2 * dy - dx;
		while (x < x2)
		{
			x++;
			if (p < 0)
				p += twoDy;
			else
			{
				y++;
				p += twoMinusDx;
			}
			setPixel(x, y);
		}
	}
	//k>=1的情况
	if (k >= 1.0)
	{
		p = dy;
		while (y < y2)
		{
			y++;
			if (p < 0)
				p += twoDx;
			else
			{
				x++;
				p += twoMinusDy;
			}
			setPixel(x, y);
		}
	}
	//0>k>-1的情况
	if (k > -1 && k < 0)
	{
		p = 2 * dy + dx;
		while (x < x2)
		{
			x++;
			if (p >= 0)
				p += twoDy;
			else
			{
				y--;
				p += twoSum;
			}
			setPixel(x, y);
		}
	}
	//k<-1的情况
	if (k <= -1)
	{
		p = 2 * dx - dy;
		while (y > y2)
		{
			y--;
			if (p >= 0)
				p -= twoDx;
			else
			{
				x++;
				p -= twoSum;
			}
			setPixel(x, y);
		}
	}
	glFlush();
}


/*  Define a four-bit code for each of the outside regions of a
 *  rectangular clipping window.
 */
const GLint winLeftBitCode = 0x1;
const GLint winRightBitCode = 0x2;
const GLint winBottomBitCode = 0x4;
const GLint winTopBitCode = 0x8;

/*  A bit-mask region code is also assigned to each endpoint of an input
 *  line segment, according to its position relative to the four edges of
 *  an input rectangular clip window.
 *
 *  An endpoint with a region-code value of 0000 is inside the clipping
 *  window, otherwise it is outside at least one clipping boundary.  If
 *  the 'or' operation for the two endpoint codes produces a value of
 *  false, the entire line defined by these two endpoints is saved
 *  (accepted).  If the 'and' operation between two endpoint codes is
 *  true, the line is completely outside the clipping window, and it is
 *  eliminated (rejected) from further processing.
 */

inline GLint inside(GLint code) { return GLint(!code); }
inline GLint reject(GLint code1, GLint code2)
{
	return GLint(code1 & code2);
}
inline GLint accept(GLint code1, GLint code2)
{
	return GLint(!(code1 | code2));
}

GLubyte encode(wcPt2D pt, wcPt2D winMin, wcPt2D winMax)
{
	GLubyte code = 0x00;

	if (pt.x < winMin.x)
		code = code | winLeftBitCode;
	if (pt.x > winMax.x)
		code = code | winRightBitCode;
	if (pt.y < winMin.y)
		code = code | winBottomBitCode;
	if (pt.y > winMax.y)
		code = code | winTopBitCode;
	return (code);
}

void swapPts(wcPt2D * p1, wcPt2D * p2)
{
	wcPt2D tmp;

	tmp = *p1; *p1 = *p2; *p2 = tmp;
}

void swapCodes(GLubyte * c1, GLubyte * c2)
{
	GLubyte tmp;

	tmp = *c1; *c1 = *c2; *c2 = tmp;
}

void lineClipCohSuth(wcPt2D winMin, wcPt2D winMax, wcPt2D &p1, wcPt2D &p2)
{
	GLubyte code1, code2;
	GLint done = false, plotLine = false;
	GLfloat m;

	while (!done) {
		code1 = encode(p1, winMin, winMax);
		code2 = encode(p2, winMin, winMax);
		if (accept(code1, code2)) {
			done = true;
			plotLine = true;
		}
		else
			if (reject(code1, code2))
				done = true;
			else {
				/*  Label the endpoint outside the display window as p1. */
				if (inside(code1)) {
					swapPts(&p1, &p2);
					swapCodes(&code1, &code2);
				}
				/*  Use slope m to find line-clipEdge intersection.  */
				if (p2.x != p1.x)
					m = (p2.y - p1.y) / (p2.x - p1.x);
				if (code1 & winLeftBitCode) {
					p1.y += (winMin.x - p1.x) * m;
					p1.x = winMin.x;
				}
				else
					if (code1 & winRightBitCode) {
						p1.y += (winMax.x - p1.x) * m;
						p1.x = winMax.x;
					}
					else
						if (code1 & winBottomBitCode) {
							/*  Need to update p1.x for nonvertical lines only.  */
							if (p2.x != p1.x)
								p1.x += (winMin.y - p1.y) / m;
							p1.y = winMin.y;
						}
						else
							if (code1 & winTopBitCode) {
								if (p2.x != p1.x)
									p1.x += (winMax.y - p1.y) / m;
								p1.y = winMax.y;
							}
			}
	}
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

	glColor3f(0.0, 1.0, 0.0);
	for (int i = 0; i < index; i += 2)
	{
		p1.x = endPoints[i][0];
		p1.y = endPoints[i][1];
		p2.x = endPoints[i + 1][0];
		p2.y = endPoints[i + 1][1];
		lineClipCohSuth(winMin, winMax, p1, p2);
		Bresenham(endPoints[i][0], endPoints[i][1], endPoints[i + 1][0], endPoints[i + 1][1]);
	}
	
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
			p1.x = x1;
			p1.y = y1;
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
		p2.x = x2;
		p2.y = y2;
		glClear(GL_COLOR_BUFFER_BIT);
		myDisplay();	//重绘
		lineClipCohSuth(winMin, winMax, p1, p2);
		Bresenham(x1, y1, x2, y2);
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

	glClear(GL_COLOR_BUFFER_BIT);
}

void menuOption(GLint select)
{
	switch (select) {
	case 1:
		index = 0;
		myDisplay();
		break;
	case 2:
		exit(0);
	default:
		break;
	}
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("裁剪算法");
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshapeFcn);

	glutMouseFunc(mouseClickFunc);
	glutPassiveMotionFunc(mouseMoveFunc);

	glutCreateMenu(menuOption);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Exit", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);


	glutMainLoop();
}