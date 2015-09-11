#define GLEW_STATIC
#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <limits>
#include <process.h>
#include <windows.h>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <map>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
using namespace std;
#include "logger.h"
////////////////////////////////////////////
void DrawString(double x, double y, double z, string string);
string otworz(string nazwa, string koniec);
string utnij(string dupa);
void zapisz();
string getFileExtension(string path);
long long unsigned checkSize(string fileName);
class Animation;
class Object;
class Subobject;
class Entity;
class Material;
class MaterialLib;
class FrustumCuller;
class Texture;
class Light;
////////////////////////////////////////////
vector<Entity*> animatedObjects;
Entity * selectedEntity;
tagPOINT *mysz_pozycja;
float modelview[16];
bool debug = true;
bool pressedRightButton = false;
bool pressedLeftButton = false;
GLfloat cx = 0;
GLfloat cy = 0;
GLfloat cx2 = 0, cy2 = 0, cz2 = 0;
int myk = 0;
int myk2 = 0;
int cameraDistance = 5;
double predkosc = 10;
int frameCounter = 0;
int frames = 0;
bool rotationEnabled = false;
GLfloat posX = -5, posY = 5, posZ = 11;
int windowHeight = 700, windowWidth = 1300;
long long unsigned totalVerticesCount = 0;
int ktoreswiatlo = 0;
int ktorapos = 0;
GLfloat kutas[] = { 0.4f, 0.4f, 0.4f, 0.4f };
int selectedEntityPos = -1;
int selectedObjectPos = 0;

HANDLE hThread;
HANDLE hThread2;
HANDLE hThread3;
struct Information {
	string x1, y1, z1, fps, speed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
};
Information info;

#include "texture.h"
#include "material.h"
#include "material_lib.h"
#include "subobject.h"
#include "object.h"
#include "entity.h"
#include "animation.h"
#include "map.h"
#include "frustum_culler.h"
#include "light.h"
#include "thread_worker.h"
#include "objects_loader.h"

map<string, Texture*> Texture::textures;
FrustumCuller* culler;
Light* light = Light::getInstance();

void resize(int width, int height) {
	const float ar = (float) width / (float) height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, culler->bottom, culler->top, culler->neardist, culler->fardist);
	culler->ar = ar;
	glMatrixMode(GL_MODELVIEW);
	windowHeight = height;
	windowWidth = width;
}

void drawObject(Entity *ob) {
	glClearColor(1, 1, 1, 1);
	GLfloat p1 = ob->px;
	GLfloat p2 = ob->py;
	GLfloat p3 = ob->pz;
	Entity *kutas = ob->parent;
	while (kutas) {
		//xd<<kutas<<endl;
		p1 += kutas->px;
		p2 += kutas->py;
		p3 += kutas->pz;
		kutas = kutas->parent;
	}
	glPushMatrix();
	glTranslatef(p1, p2, p3);
	glRotatef(ob->rx, 1, 0, 0);
	glRotatef(ob->ry, 0, 1, 0);
	glRotatef(ob->rz, 0, 0, 1);
	glScalef(ob->sx, ob->sy, ob->sz);

	Material* mtl;
	Subobject* object;
	for (unsigned j = 0; j < ob->object->subobjects.size(); j++) {
		object = ob->object->subobjects[j];
		mtl = object->mtl;

		glShadeModel(mtl->s);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtl->kat);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtl->kdt);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtl->kst);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mtl->nst);
		/*
		 if(ob->ob->mtl->td[ob->ob->ktorymtl[j]])
		 {
		 glEnable(GL_TEXTURE1);
		 glActiveTexture(GL_TEXTURE1);
		 glClientActiveTexture( GL_TEXTURE1 );
		 glEnable(GL_TEXTURE_2D);
		 glBindBuffer(GL_ARRAY_BUFFER, ob->ob->ktorybuff[j][2]);
		 glTexCoordPointer(2, GL_FLOAT, 0, 0);
		 glBindTexture(GL_TEXTURE_2D, tekstura::txtid[ob->ob->mtl->tdt[ob->ob->ktorymtl[j]]]);
		 glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		 }
		 */
		if (mtl->tkdt) {
			//	glActiveTexture(GL_TEXTURE0);
			//	glClientActiveTexture( GL_TEXTURE0 );
			glEnable(GL_TEXTURE_2D);
			glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[2]]);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glBindTexture(GL_TEXTURE_2D, mtl->tkdt->txtid);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[0]]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[1]]);
		glNormalPointer(GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Logger::log(ob->object->name);
		glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);

		if (mtl->tkdt) {
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glPopMatrix();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!frames++)
		frameCounter = clock();
	if (debug) {
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		glLoadIdentity();
		GLfloat x = -2.32;
		GLfloat y = 1.25;
		GLfloat z = -2.55;
		GLfloat dy = 0.05;
		DrawString(x, y -= dy, z, "FPS: " + info.fps);
		DrawString(x, y -= dy, z, "X: " + info.x1);
		DrawString(x, y -= dy, z, "Y: " + info.y1);
		DrawString(x, y -= dy, z, "Z: " + info.z1);
		DrawString(x, y -= dy, z, "Speed: " + info.speed);

		DrawString(x, y -= dy, z, "Ambient: " + info.amb);
		DrawString(x, y -= dy, z, "Diffuse: " + info.diff);
		DrawString(x, y -= dy, z, "Specular: " + info.spec);
		DrawString(x, y -= dy, z, "Pos: " + info.pos);
		DrawString(x, y -= dy, z, "Pos: " + info.poss);
		DrawString(x, y -= dy, z, "Wszystkie obiekty: " + info.ileob + "   Wyswietlone obiekty: " + info.ileob2);
		Object* o = Object::getObject(selectedObjectPos);
		if (o) {
			DrawString(x, y -= dy, z, "Obiekt: " + info.ob2 + "  " + o->name + "   sztuk: " + info.licznikob);
		}
		if (selectedEntityPos != -1) {
			DrawString(x, y -= dy, z, "Zaznaczony obiekt: " + info.ob + "  " + selectedEntity->object->name);
			if (Entity::getEntity(selectedEntityPos)->parent)
				DrawString(x, y -= dy, z, "Dziecko obiektu: " + selectedEntity->parent->object->name);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glLoadIdentity();
	if (selectedEntityPos > -1)
		glTranslatef(0, 0, -cameraDistance); //obrot kamery
	glRotatef(cx, 1, 0, 0);
	glRotatef(cy, 0, 1, 0);

	if (selectedEntityPos == -1)
		glTranslatef(-posX, -posY, -posZ);
	else {
		if (rotationEnabled) {
			selectedEntity->rx = cx2;
			selectedEntity->ry = -cy2;
			selectedEntity->rz = cz2;
		}
		selectedEntity->px = posX;
		selectedEntity->py = posY;
		selectedEntity->pz = posZ;
		GLfloat p1 = posX;
		GLfloat p2 = posY;
		GLfloat p3 = posZ;
		Entity* kutas = selectedEntity->parent;
		while (kutas) {
			p1 += kutas->px;
			p2 += kutas->py;
			p3 += kutas->pz;
			kutas = kutas->parent;
		}
		glTranslatef(-p1, -p2 - 5, -p3);
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	glDisable(GL_BLEND);
	//glEnable(GL_COLOR_MATERIAL);
	for (unsigned i = 0; i < Entity::solidObjectsToDisplay.size(); i++) {
		if (Entity::solidObjectsToDisplay[i]) {
			drawObject(Entity::solidObjectsToDisplay[i]);
		}
	}
	//glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	for (unsigned i = 0; i < Entity::transparentObjectsToDisplay.size(); i++) {
		if (Entity::transparentObjectsToDisplay[i]) {
			drawObject(Entity::transparentObjectsToDisplay[i]);
		}
	}

//	for(int i=0; i<Entity::allEntitiesCount(); i++) drawObject(Entity::getEntity(i));
	glutSwapBuffers();
}

void klawiaturka(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'w':
		posZ -= modelview[10] * predkosc;
		posX -= modelview[2] * predkosc;
		break;

	case 's':
		posZ += modelview[10] * predkosc;
		posX += modelview[2] * predkosc;
		break;

	case 'd':
		posX += modelview[10] * predkosc;
		posZ -= modelview[2] * predkosc;
		break;

	case 'a':
		posX -= modelview[10] * predkosc;
		posZ += modelview[2] * predkosc;
		break;

	case 'q':
		posY += predkosc;
		break;

	case 'e':
		posY -= predkosc;
		break;

	case 'o':
		debug ^= true;
		break;
	case '=':
		if (predkosc < 30)
			predkosc += 0.1;
		break;

	case '-':
		if (predkosc > 0)
			predkosc -= 0.1;
		break;

	case '{':
		if (ktoreswiatlo > 0)
			ktoreswiatlo--;
		break;

	case '}':
		if (ktoreswiatlo < 3)
			ktoreswiatlo++;
		break;

	case '[':
		if (ktorapos > 0)
			ktorapos--;
		break;

	case ']':
		if (ktorapos < 3)
			ktorapos++;
		break;

	case 39:
		switch (ktoreswiatlo) {
		case 0:
			if (light->ambient[ktorapos] < 1)
				light->ambient[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light->ambient);
			break;
		case 1:
			if (light->diffuse[ktorapos] < 1)
				light->diffuse[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light->diffuse);
			break;
		case 2:
			if (light->specular[ktorapos] < 1)
				light->specular[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light->specular);
			break;
		case 3:
			light->position[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light->position);
			break;
		}
		break;

	case ';':
		switch (ktoreswiatlo) {
		case 0:
			if (light->ambient[ktorapos] > 0)
				light->ambient[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light->ambient);
			break;
		case 1:
			if (light->diffuse[ktorapos] > 0)
				light->diffuse[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light->diffuse);
			break;
		case 2:
			if (light->specular[ktorapos] > 0)
				light->specular[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light->specular);
			break;
		case 3:
			light->position[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light->position);
			break;
		}
		break;

	case '8':
		Entity::getEntity(selectedEntityPos)->sx += predkosc;
		Entity::getEntity(selectedEntityPos)->sy += predkosc;
		Entity::getEntity(selectedEntityPos)->sz += predkosc;
		if (Entity::getEntity(selectedEntityPos)->anim) {
			Entity::getEntity(selectedEntityPos)->anim->startSx += predkosc;
			Entity::getEntity(selectedEntityPos)->anim->startSy += predkosc;
			Entity::getEntity(selectedEntityPos)->anim->startSz += predkosc;
		}
		break;

	case '5':
		Entity::getEntity(selectedEntityPos)->sx -= predkosc;
		Entity::getEntity(selectedEntityPos)->sy -= predkosc;
		Entity::getEntity(selectedEntityPos)->sz -= predkosc;
		if (Entity::getEntity(selectedEntityPos)->anim) {
			Entity::getEntity(selectedEntityPos)->anim->startSx -= predkosc;
			Entity::getEntity(selectedEntityPos)->anim->startSy -= predkosc;
			Entity::getEntity(selectedEntityPos)->anim->startSz -= predkosc;
		}
		break;

	case '4':
		if (selectedEntityPos > -1) {
			selectedEntity = Entity::getEntity(--selectedEntityPos);
			posX = Entity::getEntity(selectedEntityPos)->px;
			posY = Entity::getEntity(selectedEntityPos)->py;
			posZ = Entity::getEntity(selectedEntityPos)->pz;
			cx2 = -Entity::getEntity(selectedEntityPos)->rx;
			cy2 = -Entity::getEntity(selectedEntityPos)->ry;
		}
		break;

	case '6':
		if (selectedEntityPos < (int) Entity::allEntitiesCount() - 1) {
			selectedEntity = Entity::getEntity(++selectedEntityPos);
			posX = Entity::getEntity(selectedEntityPos)->px;
			posY = Entity::getEntity(selectedEntityPos)->py;
			posZ = Entity::getEntity(selectedEntityPos)->pz;
			cx2 = -Entity::getEntity(selectedEntityPos)->rx;
			cy2 = -Entity::getEntity(selectedEntityPos)->ry;
		}
		break;

	case '7':
		if (selectedEntityPos != -1) {
			delete Entity::getEntity(selectedEntityPos);
			Entity::setEntity(Entity::getEntity(Entity::allEntitiesCount()), selectedEntityPos);
			Entity::setEntity(NULL, Entity::allEntitiesCount());
			selectedEntityPos = -1;
			selectedEntity = NULL;
		}
		break;

	case '9':
		Entity::addEntity(new Entity(Object::getObject(selectedObjectPos)));
		if (selectedEntityPos != -1) {
			//	Entity::allObjects[Entity::allObjects.size() - 1]->parent = Entity::getEntity(selectedEntityPos);
		}
		selectedEntityPos = Entity::allEntitiesCount() - 1;
		selectedEntity = Entity::getEntity(selectedEntityPos);
		break;

	case '1':
		if (selectedObjectPos > 0)
			selectedObjectPos--;
		break;

	case '3':
		if (selectedObjectPos < Object::objectsCount() - 1)
			selectedObjectPos++;
		break;

	case '*':
		zapisz();
		break;

	}

	glutPostRedisplay();
}

void mouseButton1(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	cx -= (h / 2 - y) / 20.0;
	cy -= (w / 2 - x) / 20.0;
	if (cx > 90)
		cx = 90;
	if (cx < -90)
		cx = -90;
	rotationEnabled = false;
	myk++; //TODO
	if (myk >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk = 0;
	}
}

void mouseMotion(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	if (pressedLeftButton) {
		cx2 -= (h / 2 - y) / 15.0;
		cz2 -= (w / 2 - x) / 15.0;
	}

	if (pressedRightButton)
		cy2 -= (w / 2 - x) / 15.0;
	myk2++;
	rotationEnabled = true;
	if (myk2 >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk2 = 0;
	}
}

void mouseButton2(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		pressedLeftButton = false;
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		pressedLeftButton = true;
	if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
		pressedRightButton = false;
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		pressedRightButton = true;
}

void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0)
		cameraDistance -= predkosc;
	else
		cameraDistance += predkosc;
}

void idle(void) {
	glutPostRedisplay();
}

void zapisz() {
//	fstream zapisywacz;
//	zapisywacz.open("ustawienia/dupa.txt", ios::out);
//	int j;
//	for (int i = 0; i < ileobiektow; i++) {
//		for (j = 0; j < ileobiektow2; j++)
//			if (Entity::getEntity(i)->object == obiekty[j])
//				break;
//		zapisywacz << endl << "o " << j << endl;
//		if (Entity::getEntity(i)->anim) {
//			if (Entity::getEntity(i)->anim->startpx || Entity::getEntity(i)->anim->startpy
//					|| Entity::getEntity(i)->anim->startpz)
//				zapisywacz << "p " << Entity::getEntity(i)->anim->startpx << " "
//						<< Entity::getEntity(i)->anim->startpy << " " << Entity::getEntity(i)->anim->startpz << endl;
//			if (Entity::getEntity(i)->anim->startsx || Entity::getEntity(i)->anim->startsy
//					|| Entity::getEntity(i)->anim->startsz)
//				zapisywacz << "s " << Entity::getEntity(i)->anim->startsx << " "
//						<< Entity::getEntity(i)->anim->startsy << " " << Entity::getEntity(i)->anim->startsz << endl;
//			if (Entity::getEntity(i)->anim->startrx || Entity::getEntity(i)->anim->startry
//					|| Entity::getEntity(i)->anim->startrz)
//				zapisywacz << "r " << Entity::getEntity(i)->anim->startrx << " "
//						<< Entity::getEntity(i)->anim->startry << " " << Entity::getEntity(i)->anim->startrz << endl;
//			zapisywacz << "a" << Entity::getEntity(i)->anim->nazwa << endl;
//		} else {
//			if (Entity::getEntity(i)->px || Entity::getEntity(i)->py || Entity::getEntity(i)->pz)
//				zapisywacz << "p " << Entity::getEntity(i)->px << " " << Entity::getEntity(i)->py << " "
//						<< Entity::getEntity(i)->pz << endl;
//			if (Entity::getEntity(i)->sx || Entity::getEntity(i)->sy || Entity::getEntity(i)->sz)
//				zapisywacz << "s " << Entity::getEntity(i)->sx << " " << Entity::getEntity(i)->sy << " "
//						<< Entity::getEntity(i)->sz << endl;
//			if (Entity::getEntity(i)->rx || Entity::getEntity(i)->ry || Entity::getEntity(i)->rz)
//				zapisywacz << "r " << Entity::getEntity(i)->rx << " " << Entity::getEntity(i)->ry << " "
//						<< Entity::getEntity(i)->rz << endl;
//		}
//
//	}
}

long long unsigned checkSize(string fileName) {
	long long unsigned size;
	ifstream stream;
	stream.open(fileName.c_str(), fstream::binary);
	if (!stream.is_open())
		exit(0);
	stream.seekg(0, ios::end);
	size = stream.tellg();
	stream.close();
	return size;

}

void __cdecl animate(void *arg) {
	while (1) {
		if (selectedEntityPos == -1)
			for (unsigned i = 0; i < animatedObjects.size(); i++)
				animatedObjects[i]->anim->animuj(animatedObjects[i]);
		Sleep(15);
	}
}

void __cdecl inform(void *kutas) {
	while (1) {
		stringstream x1, y1, z1, fps, cameraSpeed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
		x1 << posX;
		y1 << posY;
		z1 << posZ;
		amb << light->ambient[0] << " " << light->ambient[1] << " " << light->ambient[2] << " " << light->ambient[3];
		diff << light->diffuse[0] << " " << light->diffuse[1] << " " << light->diffuse[2] << " " << light->diffuse[3];
		spec << light->specular[0] << " " << light->specular[1] << " " << light->specular[2] << " " << light->specular[3];
		pos << light->position[0] << " " << light->position[1] << " " << light->position[2] << " " << light->position[3];
		poss << "Swiatlo: " << ktoreswiatlo << " Pozycja: " << ktorapos;
		ob << selectedEntityPos;
		ob2 << selectedObjectPos;
		ileob << Entity::allEntitiesCount();
		ileob2 << Entity::solidObjectsToDisplay.size() + Entity::transparentObjectsToDisplay.size();
		Object* o = Object::getObject(selectedObjectPos);
		if (o) {
			licznikob << o->counter;
		}
		cameraSpeed << predkosc;
		if (clock() - frameCounter >= CLOCKS_PER_SEC) {
			fps << frames;
			info.fps = fps.str();
			frames = 0;
		}

		info.x1 = x1.str();
		info.y1 = y1.str();
		info.z1 = z1.str();
		info.speed = cameraSpeed.str();
		info.amb = amb.str();
		info.diff = diff.str();
		info.spec = spec.str();
		info.pos = pos.str();
		info.poss = poss.str();
		info.ob = ob.str();
		info.ob2 = ob2.str();
		info.ileob = ileob.str();
		info.ileob2 = ileob2.str();
		info.licznikob = licznikob.str();
		Sleep(100);
	}
}

void __cdecl sortObjects(void *dupa) {
	while (1) {
		unsigned objectsCount = Entity::allEntitiesCount();
		vector<Entity*> transparentObjects;
		vector<Entity*> solidObjects;
		for (unsigned i = 0; i < objectsCount; i++) {
			if (culler->isInViewField(i)) {
				if (Entity::getEntity(i)->object->transparent) {
					transparentObjects.push_back(Entity::getEntity(i));
				} else {
					solidObjects.push_back(Entity::getEntity(i));
				}
			}
		}

		sort(transparentObjects.begin(), transparentObjects.end(), Entity::compare);
		Entity::transparentObjectsToDisplay = transparentObjects;
		Entity::solidObjectsToDisplay = solidObjects;
		Sleep(100);
	}
}

string utnij(string kutas) { //TODO
	string dupa = "";
	int a = -1;
	for (int i = kutas.length() - 1; i >= 0; i--)
		if (kutas[i] == '/' || kutas[i] == 92) {
			a = i;
			break;
		}
	if (a == -1)
		return kutas;
	for (int i = 0; i < a; i++)
		dupa += kutas[i];
	return dupa;
}

string otworz(string nazwa, string koniec) { //TODO
	string nazwa2 = "";
	string nazwa3 = "";
	int tak = 0;
	if (nazwa[1] == ':') {
		return nazwa;
	}
	for (unsigned i = 0; i < nazwa.length(); i++) {
		nazwa3 += nazwa[i];
		if (nazwa[i] == '/' || nazwa[i] == 92)
			tak = i;
	}

	if (!tak)
		nazwa2 = "modele/" + nazwa + "/" + nazwa + koniec;
	else {
		string nazwa5 = "modele/" + nazwa3;
		string nazwa4 = "";
		for (unsigned i = tak + 1; i < nazwa.length(); i++)
			nazwa4 += nazwa[i];
		nazwa2 = nazwa5 + "/" + nazwa4 + koniec;

	}
	return nazwa2;;
}

string getFileExtension(string path) {
	int pos = path.find_last_of(".");
	return path.substr(pos, path.size());
}

void DrawString(double x, double y, double z, string string) {
	glRasterPos3d(x, y, z);
	for (unsigned i = 0; i < string.length(); i++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, (int) string[i]);
}

void checkOpenGLExtension(string roz) {
	if (!glewIsSupported(roz.c_str()))
		Logger::log(Logger::ERR + "nieobslugiwane roszerzenie " + roz);
}

int main(int argc, char** args) {
	Logger::log("Tworzenie okna...");
	glutInit(&argc, args);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Ka³nter Strajk");
	Logger::log("Inicjalizacja GLEW...");
	glewExperimental = GL_TRUE;
	ostringstream stream;
	GLenum err = glewInit();
	if (err) {
		stream << glewGetErrorString(err);
		Logger::log(stream.str());
		stream.str("");
	} else {
		Logger::log("GLEW OK");
	}
	if (!GLEW_VERSION_3_0)
		Logger::log(Logger::ERR + "masz wersje " + (char *) glGetString( GL_VERSION) + " OpenGL zamiast 4.2.0 XD");
	else {
		stream << "Wersja OpenGL: " << (char*) glGetString( GL_VERSION) << ", OK";
		Logger::log(stream.str());
		stream.str("");
	}
	checkOpenGLExtension("GL_ARB_framebuffer_object");
	checkOpenGLExtension("GL_EXT_texture_compression_s3tc");
	checkOpenGLExtension("GL_ARB_vertex_buffer_object");

	mysz_pozycja = new tagPOINT;
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(klawiaturka);
	glutPassiveMotionFunc(mouseButton1);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseButton2);
	glutMouseWheelFunc(mouseWheel);
	glutIdleFunc(idle);
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		Logger::log(Logger::ERR + "inicjalizacja SDL");
		exit(0);
	}
	Logger::log("SDL OK");
	Logger::log(Logger::LINE + "\n");

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	if (argc > 1) {
		ObjectsLoader::getInstance()->loadObjects(args[1]);
	} else {
		ObjectsLoader::getInstance()->loadObjects("ustawienia/qtas.xml");
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	culler = FrustumCuller::getInstance();
	hThread = (HANDLE) _beginthread(animate, 0, NULL);
	hThread2 = (HANDLE) _beginthread(inform, 0, NULL);
	hThread3 = (HANDLE) _beginthread(sortObjects, 0, NULL);

	glutMainLoop();
	return 0;
}
//TODO zapis
//todo przezroczystosc  gore do obiektu || rysowanie samych podobiektow
//TODO dorobienie animacji
//TODO dorobienie dzieci
//TODO wypisywanie hashu
// TODO bufferdata zajmuje 7s z 19
//TODO poprawic logi
/*x86/zlib1.dll
 x86/freeglut.dll
 x86/glew32.dll
 x86/libjpeg-9.dll
 x86/libpng16-16.dll
 x86/libtiff-5.dll
 x86/libwebp-4.dll
 x86/SDL2.dll
 x86/SDL2_image.dll

 bin/zlib1.dll
 bin/freeglut.dll
 bin/glew32.dll
 bin/libjpeg-9.dll
 bin/libpng16-16.dll
 bin/libtiff-5.dll
 bin/libwebp-4.dll
 bin/SDL2.dll
 bin/SDL2_image.dll

 */
