#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <string.h>
#include <glut.h>
#include<mmsystem.h>

int barrier = 0; //barrier idx
int VGap = 200; //vertical gap
int HGap = 250; //horizontal gap
int barrierW = 60;//barrier width
int windowW = 1220;
int windowLen = 620;
int set[3][3] = {{100,50,300},{200,100,150},{30,400,60}}; //3 sets' heights
int xpos[9]; // x pos of barriers from left
int ypos[9]; // y pos of lower barrier from top
bool reset = false; 
bool gameover = false;
bool won = false;
bool powerUp = false;
bool playedwin = false;
int radius = 22; //player radius
int xTrans = 0; //translation of rectangles in x relative to initial pos
int yTrans = 0; //translation of ball in y relative to initial pos
int BallX = 60; // x coordinate of ball
int BallStartY = windowLen/2; //initial y coordinate of ball
int cset = 0;
int ballFall = 10;
int ballJump = 80;
int screenShift = 4; //translation of barriers
int lives = 4; //in my logic they act like 3 not 4
int score = 0; 
int primOffset = 5; 
int powerUpY = 0;
int powerUpX = 0;
int randomNo = -1;
bool end = false; //game ended
int permutation [6][3]= { {1,2,3},{1,3,2},{2,1,3},{2,3,1},{3,1,2},{3,2,1} }; //3! permutations to choose from

void print(int x, int y, char* string)
{
	int len, i;

	glRasterPos2f(x, y);

	len = (int)strlen(string);

	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}


void drawTriangle(int x, int y, int w,int h) {
	glBegin(GL_POLYGON);
	glColor3f(0.7, 0.1, 0.4);
	glVertex2f(x, y);
	glVertex2f(x + w,y);
	glVertex2f(x + w/2, y + h);
	glEnd();
}

void drawRectangle(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);

	glVertex2f(x, y - primOffset);
	glVertex2f(x + w, y - primOffset);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();

}

void drawPowerUp(int x, int y, float r) {

	glColor3f(0.3, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
	glColor3f(0.8, 0.2, 0.5);
	drawRectangle(x -r/2, y - r / 3, r, r / 2);
	
}

void drawPlayer(int x, int y, float r) {

	glColor3f(0.8, 1, 0);
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
	glColor3f(0.3, 0, 0);
	drawTriangle(x+r/1.5, y-r/2, r/2, r/2);
	glColor3f(0.8, 0.5, 0.5);
	drawRectangle(x -r , y - r / 2, r, r/2);

}

void drawBarrier(int x, int y, int w, int h,bool white) {
	
	drawRectangle(x, y, w, h);
	glBegin(GL_POLYGON);
	if(white&&powerUp)
		glColor3f(1, 1, 1);
	else
		glColor3f(0.3, 0, 0);

	glVertex2f(x+primOffset, y);
	glVertex2f(x + w-primOffset, y);
	glVertex2f(x + w-primOffset, y + h-primOffset);
	glVertex2f(x+primOffset, y + h-primOffset);
	glEnd();
	glColor3f(0.7, 0.1, 0.4);
	drawTriangle(x, y, w-2, 10);
	
	
}

void createSet(int setNo,int i) {
	for (int x = 0; x < 3;x++) {
		int len1 = set[setNo][x];
		
		if (x == 0 && i == 1) {
			if (!powerUp) {
				drawPowerUp(4 * HGap + barrierW / 2, len1 + (VGap / 2), radius);
				powerUpY = len1 + (VGap / 2);
				powerUpX = 4 * HGap + barrierW / 2;
			}
		}
		drawBarrier((3 * i + 1 + x) * HGap, 0, barrierW, len1,i==1);
		drawBarrier((3 * i + 1 + x) * HGap, len1 + VGap, barrierW, windowLen - VGap - len1,i==1);
		xpos[3 * i + x] = (3 * i + 1 + x) * HGap;
		ypos[3 * i + x] = len1;
	}
}

void orderSets() {
	srand(time(0));
	if(randomNo==-1) 
		randomNo = rand() % 6;
	for (int i = 0;i < 3;i++){
		int setNo = permutation[randomNo][i];
		glColor3f(0.7, 0.1, 0.4);		
		createSet(setNo, i);

	}
}

void powerUpdetection() {
	if (abs(BallStartY + yTrans - powerUpY)<2*radius) {
		if (abs(BallX - powerUpX -xTrans) < 2*radius) {
			powerUp = true;	
			PlaySound(TEXT("sounds/powerUp.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
	}
}

void detectCollision() {
	
		bool collision = false;

		if (xpos[barrier] + xTrans + barrierW < BallX - radius) { //after barrier
			if (barrier == 5)
				powerUp = false;
			barrier++;
		}
		if (!powerUp) {
		if (xpos[barrier] + xTrans < BallX + radius) { //within barrier
			if (ypos[barrier] > BallStartY + yTrans - radius) {
				collision = true;
			}
			else if (ypos[barrier] + VGap < BallStartY + yTrans + radius) {
				collision = true;
			}
		}
		
		if (collision) {
			//printf("collided");
			lives--;
			if (lives == 0) {
				glColor3f(0.3, 0, 0);
				printf("GAME OVER");
				gameover = true;
				PlaySound(TEXT("sounds/gameOver.wav"), NULL, SND_FILENAME | SND_ASYNC);
				end = true;
			}
			else {
				glPushMatrix();
				glTranslated(0, 0, 0);
				drawPlayer(BallX, BallStartY, radius);
				glPopMatrix();
				reset = true;
				if(lives!=3)
					PlaySound(TEXT("sounds/ouch.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			collision = false;
		}
	}
	if (BallX - radius > barrierW + xpos[8]+xTrans) {
		won = true;
	}
	if ((BallStartY + yTrans + radius > windowLen || BallStartY - radius + yTrans < 0)&&!won) {
		gameover = true;
		end = true;
		PlaySound(TEXT("sounds/gameOver.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
}


void timer(int val)
{
	glutPostRedisplay();							
	glutTimerFunc(50, timer, 0);
	if (!gameover) {
		detectCollision();
		if(!powerUp)
			powerUpdetection();
	}
}

void scoreTime(int val)
{
	glutTimerFunc(333, scoreTime, 0);
	score += 1;
}


void Key(unsigned char key, int x, int z) {
	if (key == 'a') 
		yTrans+=ballJump ;
	
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (!end) {
		if (reset) {
			xTrans += 120;
			glPushMatrix();
			glColor3f(1, 1, 0);
			glTranslated(0, 0, 0);
			drawPlayer(BallX, BallStartY, radius);
			glPopMatrix();
			yTrans = 0;
			reset = false;
		}
		else {
			glPushMatrix();
			glColor3f(1, 1, 0);
			glTranslated(0, yTrans, 0);
			drawPlayer(BallX, BallStartY, radius);
			glPopMatrix();
		}
		glPushMatrix();
		glTranslated(xTrans, 0, 0);
		glColor3f(0.7, 0.1, 0.4);
		orderSets();
		glPopMatrix();
		xTrans -= screenShift;
		yTrans -= ballFall;
		char* l[20];
		glColor3f(1, 1, 1);
		sprintf((char*)l, "Lives: %i", lives);
		print(windowW / 2, windowLen - 100, (char*)l);
		
		sprintf((char*)l, "SCORE: %i", score);
		print(windowW / 2, windowLen - 130, (char*)l);

	}
	else if (won && !playedwin) {
		char* l[20];
		glColor3f(1, 1, 1);
		sprintf((char*)l, "YOU WIN!!");
		PlaySound(TEXT("sounds/win.wav"), NULL, SND_FILENAME | SND_ASYNC);
		playedwin = true;
		end = true;
		print(windowW / 2, windowLen - 100, (char*)l);
	}
	else if(!won && gameover){
		char* l[20];
		glColor3f(1, 1, 1);
		sprintf((char*)l, "GAME OVER!!");
		print(windowW/2, windowLen-100, (char*)l);
	}
	glFlush();
}

void main(int argc, char** argr) {
	glutInit(&argc, argr);
	glutInitWindowSize(windowW, windowLen);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Flappy Bird");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Key);
	glutTimerFunc(0, timer, 0);		
	glutTimerFunc(0, scoreTime , 0);		
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gluOrtho2D(0.0, windowW, 0.0, windowLen);
	glutMainLoop();
	
}
