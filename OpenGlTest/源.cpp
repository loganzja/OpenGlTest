//鼠标左键 拖动可控制物体旋转，x,y,z键控制物体分别绕x轴，y轴，z轴旋转
//功能键PageUp 控制物体放大，PageDown控制物体缩小
//↑↓←→控制物体上下左右移动
//


#include <GL/glut.h>
#include <iostream>

const GLint winWidth = 500, winHeight = 500;

typedef GLfloat Matrix4x4[4][4];

Matrix4x4 eyeMatrix4x4 = { 1,0,0,0,
						  0,1,0,0,
						  0,0,1,5,
						  0,0,0,1 };	//照相机位置的4x4矩阵
Matrix4x4 centerMatrix4x4 = { 1,0,0,0,
							  0,1,0,0,
							  0,0,1,0,
							  0,0,0,1 };	//观察中心的4x4矩阵

double eye[] = { eyeMatrix4x4[0][3], eyeMatrix4x4[1][3], eyeMatrix4x4[2][3] };	//眼睛的位置
double center[] = { centerMatrix4x4[0][3], centerMatrix4x4[1][3], centerMatrix4x4[2][3] };//观察中心的位置
double place[] = { 0, 1, 0 };//脑袋的朝向，默认朝向y轴正方向

double angle_x = 0;	//x轴旋转角度参数
double angle_y = 0;	//y轴旋转角度参数
double angle_z = 0;	//z轴旋转角度参数

double times = 1.0;	//缩放的倍数
const double move_x = 0;	//x轴默认移动距离0
const double move_y = 0;	//y轴默认移动距离0
const double move_z = 0;	//z轴默认移动距离0

GLint oldx = 0;	//鼠标控制三维物体移动过程中，每一次都要计算位移量，初始点坐标更新
GLint oldy = 0;

int project_mode = 0;	//投影模式，0代表透视投影，1代表正投影

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);	//set color to white
}
/*平移一个三维物体，通过矩阵变换实现有一定难度，但是可以通过矩阵变换改变照相机和观察中心的位置，
 *只要二者同步移动，同样可以实现三维物体移动的效果
 */
void updateEyeAndCenter()	//照相机和观察中心位置更新
{
	for (int i = 0; i < 3; i++)
	{
		eye[i] = eyeMatrix4x4[i][3];
		center[i] = centerMatrix4x4[i][3];
	}
}

void matrix4x4SetIdentity(Matrix4x4 matIdent4x4)	//构造一个4x4单位矩阵
{
	GLint row, col;

	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matIdent4x4[row][col] = (row == col);
}

void matrix4x4PreMultiply(Matrix4x4 m1, Matrix4x4 m2)	//m2 = m1*m2
{
	GLint row, col;
	Matrix4x4 matTemp;

	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matTemp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] * m2[1][col] + m1[row][2] * m2[2][col] + m1[row][3] * m2[3][col];
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			m2[row][col] = matTemp[row][col];
}

void translate3D(GLfloat tx, GLfloat ty, GLfloat tz)	//3维平移
{
	Matrix4x4 matTransl3D;

	matrix4x4SetIdentity(matTransl3D);

	matTransl3D[0][3] = tx;
	matTransl3D[1][3] = ty;
	matTransl3D[2][3] = tz;

	matrix4x4PreMultiply(matTransl3D, eyeMatrix4x4);
	matrix4x4PreMultiply(matTransl3D, centerMatrix4x4);
	updateEyeAndCenter();
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除颜色缓存和深度缓存
	glColor3f(1.0, 0.0, 0.0);	//画笔颜色为红色
	glLoadIdentity();	//初始化单位矩阵
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], place[0], place[1], place[2]);

	glEnable(GL_DEPTH_TEST);  //开启深度测试
	glEnable(GL_LIGHTING); //开启光照模式
	GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 }; // 定义颜色
	GLfloat light_pos[] = { 5,5,5,1 };  //定义光源位置

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //设置第0号光源的光照位置
	glLightfv(GL_LIGHT0, GL_AMBIENT, red); //设置第0号光源多次反射后的光照颜色（环境光颜色）
	glEnable(GL_LIGHT0);					//开启第0号光源

	glScalef(times, times, times);
	glRotated(angle_x, 1, 0, 0);	//x轴旋转角度
	glRotated(angle_y, 0, 1, 0);	//y轴旋转角度
	glRotated(angle_z, 0, 0, 1);	//z轴旋转角度
	glutSolidTeapot(2);

	glutSwapBuffers();
}

void keyboard(GLubyte key, GLint xMouse, GLint yMouse)
{
	switch (key)
	{
	case 'x':
		angle_x = (int)(angle_x + 1) % 360;
		break;
	case 'y':
		angle_y = (int)(angle_y + 1) % 360;
		break;
	case 'z':
		angle_z = (int)(angle_z + 1) % 360;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void scaleFcn(GLint specialKey, GLint xMouse, GLint yMOuse)
{
	switch (specialKey)
	{
	case GLUT_KEY_PAGE_UP:
		times += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (times <= 0.1)	times = 0.1;
		else	times -= 0.1;
		break;
	case GLUT_KEY_UP:
		translate3D(move_x, -0.1, move_z);
		break;
	case GLUT_KEY_DOWN:
		translate3D(move_x, 0.1, move_z);
		break;
	case GLUT_KEY_RIGHT:
		translate3D(-0.1, move_y, move_z);
		break;
	case GLUT_KEY_LEFT:
		translate3D(0.1, move_y, move_z);
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			oldx = x;//当左键按下时记录鼠标坐标  
			oldy = y;
		}
	}
}

void motion(GLint xMouse, GLint yMouse)
{
	GLint delta_x = xMouse - oldx;
	GLint delta_y = yMouse - oldy;

	angle_y += (360 * delta_x / winWidth);
	angle_x += (360 * delta_y / winHeight);
	//angle_z += (360 * delta_y / winHeight);
	oldx = xMouse;
	oldy = yMouse;
	glutPostRedisplay();
}

void reshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);	//视图窗口大小
	glMatrixMode(GL_PROJECTION);	//设置为投影模式
	glLoadIdentity();
	if(project_mode == 0)
		gluPerspective(90, (GLdouble)newWidth / (GLdouble)newHeight, 1, 100);	//透视投影
	else
		glOrtho(-10, 10, -10, 10, -100, 100);	//平行投影
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], place[0], place[1], place[2]);	//照相机位置
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);//初始化显示模式:RGB颜色模型，深度测试，双缓冲
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("三维物体");
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshapeFcn);

	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(scaleFcn);
	//glutIdleFunc(myDisplay);
	glutMainLoop();
	return 0;
}