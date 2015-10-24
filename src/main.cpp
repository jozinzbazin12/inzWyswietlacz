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
void DrawString(GLfloat x, GLfloat y, GLfloat z, string string);
string getPath(string path);
string getRealPath(string path, string actualPath);
void zapisz();
void end();
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
bool pressedMiddleButton = false;
GLfloat cx = 0;
GLfloat cy = 0;
GLfloat cx2 = 0, cy2 = 0, cz2 = 0;
int motionCounter = 0;
int motionCounter2 = 0;
int cameraDistance = 5;
double predkosc = 10;
int frameCounter = 0;
int frames = 0;
bool rotationEnabled = false;
GLfloat posX = -5, posY = 5, posZ = 11;
int windowHeight = 700, windowWidth = 1300;
long long unsigned totalVerticesCount = 0;
int selectedEntityPos = -1;
int selectedObjectPos = 0;
vector<Entity*> transparentObjects;
vector<Entity*> solidObjects;

HANDLE animateThread;
HANDLE informThread;
HANDLE sortThread;
struct Information {
	string x1, y1, z1, fps, speed, amb, diff, spec, pos, ob, ob2, ileob, ileob2, licznikob;
};
Information info;

class TreeNode;
#include "texture.h"
#include "material.h"
#include "material_lib.h"
#include "subobject.h"
#include "object.h"
#include "entity.h"
#include "animation.h"
#include "map_material.h"
#include "map.h"
#include "light.h"
#include "tree.h"
#include "frustum_culler.h"
#include "thread_worker.h"
#include "objects_loader.h"
#include "console.h"

void TreeNode::addObject(Entity* e) {
	TreeNode* node = getChild(e);
	while (node->level <= node->LEVELS) {
		node = node->getChild(e);
	}
	TreeLeaf* leaf = (TreeLeaf*) node;
	leaf->addObject(e);
}

void Entity::addEntity(Entity* entity) {
	WaitForSingleObject(mutex, INFINITE);
	allObjects.push_back(entity);
	objects->addObject(entity);
	ReleaseMutex(mutex);
}

map<string, Texture*> Texture::textures;
FrustumCuller* culler;
Light* light = Light::getInstance();
Console* console;

void resize(int width, int height) {
	const float ar = (float) width / (float) height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	culler->ar = ar;
	culler->commit();
	glMatrixMode(GL_MODELVIEW);
	windowHeight = height;
	windowWidth = width;
}

void drawObject(Entity *ob) {
	glClearColor(1, 1, 1, 1);
	GLfloat p1 = ob->px; //FIXME jakims cudem ni mo
	GLfloat p2 = ob->py;
	GLfloat p3 = ob->pz;
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
			glBindBuffer(GL_ARRAY_BUFFER, object->texture);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glBindTexture(GL_TEXTURE_2D, mtl->tkdt->txtid);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glBindBuffer(GL_ARRAY_BUFFER, object->vertices);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, object->normals);
		glNormalPointer(GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);
		if (mtl->tkdt) {
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glPopMatrix();
}

void displayDebug() {
	glDisable(GL_LIGHTING);
	glColor3f(0, 0, 0);
	glLoadIdentity();
	GLfloat x = -windowWidth / 560.5;
	GLfloat y = windowHeight / 560.0;
	GLfloat z = -2.52;
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
	DrawString(x, y -= dy, z, "Wszystkie obiekty: " + info.ileob + "   Wyswietlone obiekty: " + info.ileob2);
	Object* o = Object::getObject(selectedObjectPos);
	if (o) {
		DrawString(x, y -= dy, z, "Obiekt: " + info.ob2 + "  " + o->name + "   sztuk: " + info.licznikob);
	}
	if (selectedEntityPos != -1) {
		DrawString(x, y -= dy, z, "Zaznaczony obiekt: " + info.ob + "  " + selectedEntity->object->name);
	}
	y -= dy * 4;
	for (int i = 0; i < console->lineNumber - 1; i++) {
		DrawString(x, y -= dy, z, console->lines[i]);
	}
	DrawString(x, y -= dy * 2, z, console->lines[console->lineNumber - 1]);
	if (console->typing) {
		DrawString(x + console->pointer / 30.8, y - 0.001, z, "_");
	}
	glEnable(GL_LIGHTING);
	glColor3f(0.5, 0.5, 0.5);
}
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (light->ready) {
		light->commit();
		light->setReady(false);
	}
	if (!frames++) {
		frameCounter = clock();
	}
	if (debug) {
		displayDebug();
	} else {
		glColor3f(0.5, 0.5, 0.5);
	}
	glLoadIdentity();
	if (selectedEntityPos > -1) {
		glTranslatef(0, 0, -cameraDistance); //obrot kamery
	}
	glRotatef(cx, 1, 0, 0);
	glRotatef(cy, 0, 1, 0);

	if (selectedEntityPos == -1) {
		glTranslatef(-posX, -posY, -posZ);
	} else {
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
		glTranslatef(-p1, -p2 - 5, -p3);
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	unsigned size = Entity::solidObjectsToDisplay.size();
	vector<Entity*> entities = Entity::solidObjectsToDisplay;
	glDisable(GL_BLEND);
	for (unsigned i = 0; i < size; i++) {
		drawObject(entities[i]);

	}
	size = Entity::transparentObjectsToDisplay.size();
	entities = Entity::transparentObjectsToDisplay;
	glEnable(GL_BLEND);
	for (unsigned i = 0; i < size; i++) {
		drawObject(entities[i]);
	}

//	for(int i=0; i<Entity::allEntitiesCount(); i++) drawObject(Entity::getEntity(i));
	glutSwapBuffers();
}

void specialKeys(int key, int x, int y) {
	if (console->typing && debug) {
		console->typeSpecial(key);
	}
}
void klawiaturka(unsigned char key, int x, int y) {
	if (console->typing && debug) {
		console->type(key);
	} else {
		Entity* e;
		switch (key) {
		case '`':
			console->typing = true;
			break;
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

		case '8':
			e = Entity::getEntity(selectedEntityPos);
			e->sx += predkosc;
			e->sy += predkosc;
			e->sz += predkosc;
			if (e->anim) {
				e->anim->startSx += predkosc;
				e->anim->startSy += predkosc;
				e->anim->startSz += predkosc;
			}
			break;

		case '5':
			e = Entity::getEntity(selectedEntityPos);
			e->sx -= predkosc;
			e->sy -= predkosc;
			e->sz -= predkosc;
			if (e->anim) {
				e->anim->startSx -= predkosc;
				e->anim->startSy -= predkosc;
				e->anim->startSz -= predkosc;
			}
			break;

		case '4':
			if (selectedEntityPos > -1) {
				selectedEntity = Entity::getEntity(selectedEntityPos--);
				posX = selectedEntity->px;
				posY = selectedEntity->py;
				posZ = selectedEntity->pz;
				cx2 = -selectedEntity->rx;
				cy2 = -selectedEntity->ry;
			}
			break;

		case '6':
			if (selectedEntityPos < (int) Entity::allEntitiesCount() - 1) {
				selectedEntity = Entity::getEntity(++selectedEntityPos);
				posX = selectedEntity->px;
				posY = selectedEntity->py;
				posZ = selectedEntity->pz;
				cx2 = -selectedEntity->rx;
				cy2 = -selectedEntity->ry;
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
			if (selectedObjectPos > 0) {
				selectedObjectPos--;
			}
			break;

		case '3':
			if (selectedObjectPos < Object::objectsCount() - 1) {
				selectedObjectPos++;
			}
			break;

		case '*':
			zapisz();
			break;
		}
	}

	glutPostRedisplay();
}

void mouseMotion(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	cx -= (h / 2 - y) / 10.0;
	cy -= (w / 2 - x) / 10.0;
	if (cx > 90) {
		cx = 90;
	}
	if (cx < -90) {
		cx = -90;
	}
	rotationEnabled = false;
	motionCounter++;
	if (motionCounter >= 2) {
		glutWarpPointer(w / 2, h / 2);
		motionCounter = 0;
	}
}

void mousePressedMotion(int x, int y) {
	if (pressedLeftButton) {
		cx2 -= (windowHeight / 2 - y) / 7.0;
		cz2 -= (windowWidth / 2 - x) / 7.0;
	}

	if (pressedRightButton) {
		cy2 -= (windowWidth / 2 - x) / 7.0;
	}
	motionCounter2++;
	rotationEnabled = true;
	if (motionCounter2 >= 2 && !pressedMiddleButton) {
		glutWarpPointer(windowWidth / 2, windowHeight / 2);
		motionCounter2 = 0;
	}
}

void mousePressed(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		pressedLeftButton = false;
	}
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		pressedLeftButton = true;
	}
	if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON) {
		pressedRightButton = false;
	}
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
		pressedRightButton = true;
	}
	if (state == GLUT_UP && button == GLUT_MIDDLE_BUTTON) {
		pressedMiddleButton = false;
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) {
		pressedMiddleButton = true;
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		cameraDistance -= predkosc;
	} else {
		cameraDistance += predkosc;
	}
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
	if (!stream.is_open()) {
		exit(0);
	}
	stream.seekg(0, ios::end);
	size = stream.tellg();
	stream.close();
	return size;

}

void __cdecl animate(void *arg) {
	while (1) {
		if (selectedEntityPos == -1) {
			for (unsigned i = 0; i < animatedObjects.size(); i++) {
				animatedObjects[i]->anim->animuj(animatedObjects[i]);
			}
		}
		Sleep(15);
	}
}

void __cdecl inform(void *kutas) {
	while (1) {
		stringstream x1, y1, z1, fps, cameraSpeed, amb, diff, spec, pos, ob, ob2, ileob, ileob2, licznikob;
		x1 << posX;
		y1 << posY;
		z1 << posZ;
		amb << light->ambient[0] << " " << light->ambient[1] << " " << light->ambient[2] << " " << light->ambient[3];
		diff << light->diffuse[0] << " " << light->diffuse[1] << " " << light->diffuse[2] << " " << light->diffuse[3];
		spec << light->specular[0] << " " << light->specular[1] << " " << light->specular[2] << " " << light->specular[3];
		pos << light->position[0] << " " << light->position[1] << " " << light->position[2] << " " << light->position[3];
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
		info.ob = ob.str();
		info.ob2 = ob2.str();
		info.ileob = ileob.str();
		info.ileob2 = ileob2.str();
		info.licznikob = licznikob.str();
		Sleep(100);
	}
}

void checkVisibility(TreeNode* n) {
	if (n->level == n->LEVELS + 1) {
		TreeLeaf* leaf = (TreeLeaf*) n;
		vector<Entity*> vec = leaf->entities;
		for (unsigned i = 0; i < vec.size(); i++) {
			Entity* e = vec[i];
			if (culler->isInViewField(e)) {
				if (e->object->transparent) {
					transparentObjects.push_back(e);
				} else {
					solidObjects.push_back(e);
				}
			}
		}
	} else {
		for (int i = 0; i < 4; i++) {
			TreeNode* node = n->children[i];
			if (node && culler->isInViewField(node)) {
				checkVisibility(node);
			}
		}
	}
}

void __cdecl sortObjects(void *arg) {
	while (1) {
//		unsigned objectsCount = Entity::allEntitiesCount();
//		for (unsigned i = 0; i < objectsCount; i++) {
//			Entity* e = Entity::getEntity(i);
//			if (culler->isInViewField(e)) {
//				if (e->object->transparent) {
//					transparentObjects.push_back(e);
//				} else {
//					solidObjects.push_back(e);
//				}
//			}
//		}
		transparentObjects.clear();
		solidObjects.clear();
		if (Entity::objects) {
			checkVisibility(Entity::objects);
			sort(transparentObjects.begin(), transparentObjects.end(), Entity::compare);
			Entity::transparentObjectsToDisplay = transparentObjects;
			Entity::solidObjectsToDisplay = solidObjects;
		}
		Sleep(100);
	}
}

string getRealPath(string path, string actualPath) {
	if (path.find(":") == 1) {
		return path;
	} else {
		return actualPath + path;
	}
}

string getPath(string path) {
	int index = path.find_last_of("/");
	int index2 = path.find_last_of("\\");
	index = max(index, index2);
	return path.substr(0, index + 1);
}

string getFileExtension(string path) {
	int pos = path.find_last_of(".");
	return path.substr(pos, path.size());
}

void DrawString(GLfloat x, GLfloat y, GLfloat z, string string) {
	glRasterPos3f(x, y, z);
	for (unsigned i = 0; i < string.length(); i++) {
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, (int) string[i]);
	}
}

void checkOpenGLExtension(string roz) {
	if (!glewIsSupported(roz.c_str())) {
		Logger::log(Logger::ERR + "nieobslugiwane roszerzenie " + roz);
	}
}

void end() {
	TerminateThread(animateThread, 0);
	TerminateThread(informThread, 0);
	TerminateThread(sortThread, 0);
	ObjectsLoader::getInstance()->terminate();
	exit(0);
}

int main(int argc, char** args) {
	atexit(end);
	Logger::log("Tworzenie okna...");
	glutInit(&argc, args);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Ka�nter Strajk");
	glutSetCursor(GLUT_CURSOR_NONE);
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
	if (!GLEW_VERSION_3_0) {
		Logger::log(Logger::ERR + "masz wersje " + (char *) glGetString(GL_VERSION) + " OpenGL zamiast 4.2.0 XD");
	} else {
		stream << "Wersja OpenGL: " << (char*) glGetString(GL_VERSION) << ", OK";
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
	glutSpecialFunc(specialKeys);
	glutPassiveMotionFunc(mouseMotion);
	glutMotionFunc(mousePressedMotion);
	glutMouseFunc(mousePressed);
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
	glDepthMask(true);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glEnable(GL_COLOR_MATERIAL);
	light->commit();
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	console = new Console(10);
	if (argc > 1) {
		ObjectsLoader::getInstance()->loadObjects(args[1]);
	} else {
		ObjectsLoader::getInstance()->loadObjects("ustawienia/qtas.xml");
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	culler = FrustumCuller::getInstance();
	animateThread = (HANDLE) _beginthread(animate, 0, NULL);
	informThread = (HANDLE) _beginthread(inform, 0, NULL);
	sortThread = (HANDLE) _beginthread(sortObjects, 0, NULL);
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
//TODO drzewa czworkowe, max 100k obiektow
// TODO ustawianie na krawedziach
//todo w szybie wylaczyc z buffer
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
