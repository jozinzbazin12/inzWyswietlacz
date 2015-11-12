#define GLEW_STATIC
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
double getLength2D(double* p1, double* p2);
double getLength2D(double x1, double y1, double x2, double y2);
double getLength3D(double* p1, double* p2);
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
Entity* selectedEntity = NULL;
Entity* deleted = NULL;
tagPOINT *mysz_pozycja;
float modelview[16];
double empty[3] = { 0, 0, 0 };
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
double cameraSpeed = 10;
int frameCounter = 0;
int frames = 0;
int lod = 150;
bool rotationEnabled = false;
GLfloat posX = -5, posY = 5, posZ = 11;
int windowHeight = 700, windowWidth = 1300;
long long unsigned totalVerticesCount = 0;
int selectedObjectPos = 0;
list<Entity*> transparentObjects;
list<Entity*> solidObjects;

HANDLE informThread;
HANDLE sortThread;
HANDLE listsMutex;
struct Information {
	string x, y, z, fps, speed, selectedObject, allEntities, displayedEntities, objectsCount;
};
Information info;

class TreeNode;
#include "texture.h"
#include "material.h"
#include "material_lib.h"
#include "subobject.h"
#include "object.h"
#include "cullable.h"
#include "entity.h"
#include "map_material.h"
#include "map.h"
#include "light.h"
#include "tree.h"
#include "frustum_culler.h"
#include "thread_worker.h"
#include "objects_loader.h"
#include "console.h"
#include "console_actions.h"

void Entity::addEntity(Entity* entity) {
	WaitForSingleObject(mutex, INFINITE);
	objects->addObject(entity);
	ReleaseMutex(mutex);
}
void Entity::deleteFromTree() {
	objects->deleteOb(this);
	deleted = this;
}

void Console::init() {
	commands["speed"] = new SpeedAction();
	commands["goto"] = new GotoAction();
	commands["light_ambient"] = new LightAmbientAction();
	commands["light_diffuse"] = new LightDiffuseAction();
	commands["light_specular"] = new LightSpecularAction();
	commands["light_position"] = new LightPositionAction();
	commands["range"] = new RangeAction();
	commands["lod"] = new LODAction();
	commands["edit"] = new EditEntityAction();
	commands["end"] = new EndEditEntityAction();
	Action* exit = new ExitAction();
	commands["quit"] = exit;
	commands["exit"] = exit;
	commands["scale"] = new ScaleEntityAction();
	commands["rotate"] = new RotateEntityAction();
	commands["new"] = new NewEntityAction();
	commands["delete"] = new DeleteEntityAction();
}

void Console::parse() {
	vector<string> result = split(lines[lineNumber - 1]);
	Action* action = commands[result[0]];
	if (action) {
		action->execute(this, result);
	} else {
		nextLine();
		lines[lineNumber - 2] = "Unknown command";
	}
}

FrustumCuller* culler;
Light* light = Light::getInstance();
Console* console;

void resize(int width, int height) {
	culler->commit(width, height);
}

void drawObject(Entity *ob) {
	if (ob == deleted) {
		deleted = NULL;
		return;
	}
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

		if (mtl->tkdt) {
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
	DrawString(x, y -= dy, z, "X: " + info.x);
	DrawString(x, y -= dy, z, "Y: " + info.y);
	DrawString(x, y -= dy, z, "Z: " + info.z);
	DrawString(x, y -= dy, z, "Speed: " + info.speed);

	DrawString(x, y -= dy, z, "All entities: " + info.allEntities + "   Displayed entities: " + info.displayedEntities);
	Object* o = Object::getObject(selectedObjectPos);
	if (o) {
		DrawString(x, y -= dy, z, "Object: " + o->name + "   pieces: " + info.objectsCount);
	}
	if (selectedEntity) {
		DrawString(x, y -= dy, z, "Selected object: " + info.selectedObject);
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
	if (selectedEntity) {
		glTranslatef(0, 0, -cameraDistance); //obrot kamery
	}
	glRotatef(cx, 1, 0, 0);
	glRotatef(cy, 0, 1, 0);

	if (!selectedEntity) {
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
		glTranslatef(-posX, -posY - 5, -posZ);
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	WaitForSingleObject(listsMutex, INFINITE);
	list<Entity*> entities = Entity::solidObjectsToDisplay;
	ReleaseMutex(listsMutex);
	glDisable(GL_BLEND);
	for (list<Entity*>::iterator i = entities.begin(); i != entities.end(); ++i) {
		drawObject(*i);
	}

	WaitForSingleObject(listsMutex, INFINITE);
	entities = Entity::transparentObjectsToDisplay; //FIXME dodac mutex?
	ReleaseMutex(listsMutex);
	glEnable(GL_BLEND);
	for (list<Entity*>::iterator i = entities.begin(); i != entities.end(); ++i) {
		drawObject(*i);
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
		key = tolower(key);
		switch (key) {
		case '`':
			console->typing = true;
			break;

		case 'w':
			posZ -= modelview[10] * cameraSpeed;
			posX -= modelview[2] * cameraSpeed;
			break;

		case 's':
			posZ += modelview[10] * cameraSpeed;
			posX += modelview[2] * cameraSpeed;
			break;

		case 'd':
			posX += modelview[10] * cameraSpeed;
			posZ -= modelview[2] * cameraSpeed;
			break;

		case 'a':
			posX -= modelview[10] * cameraSpeed;
			posZ += modelview[2] * cameraSpeed;
			break;

		case 'q':
			posY += cameraSpeed;
			break;

		case 'e':
			posY -= cameraSpeed;
			break;

		case 'o':
			debug ^= true;
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
		cameraDistance -= cameraSpeed;
	} else {
		cameraDistance += cameraSpeed;
	}
}

void idle(void) {
	glutPostRedisplay();
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

void __cdecl inform(void *arg) {
	while (1) {
		stringstream xs, ys, zs, fps, cameraSpeedS, selectedObjectS, entitiesCountS, displayedEntitiesS, objectsCountS;
		xs << posX;
		ys << posY;
		zs << posZ;
		if (selectedEntity) {
			selectedObjectS << selectedEntity->object->name;
		}
		entitiesCountS << Entity::entitiesCount;
		WaitForSingleObject(listsMutex, INFINITE);
		displayedEntitiesS << Entity::solidObjectsToDisplay.size() + Entity::transparentObjectsToDisplay.size();
		ReleaseMutex(listsMutex);
		Object* o = Object::getObject(selectedObjectPos);
		if (o) {
			objectsCountS << o->counter;
		}
		cameraSpeedS << cameraSpeed;
		if (clock() - frameCounter >= CLOCKS_PER_SEC) {
			fps << frames;
			info.fps = fps.str();
			frames = 0;
		}

		info.x = xs.str();
		info.y = ys.str();
		info.z = zs.str();
		info.speed = cameraSpeedS.str();
		info.selectedObject = selectedObjectS.str();
		info.allEntities = entitiesCountS.str();
		info.displayedEntities = displayedEntitiesS.str();
		info.objectsCount = objectsCountS.str();
		Sleep(100);
	}
}

void addEntityIfVisible(Entity* e) {
	if (e != deleted && culler->isInViewField(e)) {
		if (e->object->transparent) {
			transparentObjects.push_back(e);
		} else {
			solidObjects.push_back(e);
		}
	}
}

void checkVisibility(TreeNode* n) {
	for (unsigned i = 0; i < n->getSize(); i++) {
		Entity* e = n->getEntity(i);
		addEntityIfVisible(e);
	}

	for (int i = 0; i < 4; i++) {
		TreeNode* node = n->children[i];
		if (node && culler->isInViewField(node)) {
			checkVisibility(node);
		}
	}
}

void __cdecl sortObjects(void *arg) {
	while (1) {
		transparentObjects.clear();
		solidObjects.clear();
		for (list<Entity*>::iterator it = Entity::movingObjects.begin(); it != Entity::movingObjects.end(); ++it) {
			addEntityIfVisible(*it);
		}
		if (Entity::objects && culler->isInViewField(Entity::objects)) {
			checkVisibility(Entity::objects);
			WaitForSingleObject(listsMutex, INFINITE);
			transparentObjects.sort(Entity::compare);
			Entity::transparentObjectsToDisplay = transparentObjects;
			Entity::solidObjectsToDisplay = solidObjects;
			ReleaseMutex(listsMutex);
		}
		Sleep(50);
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
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int) string[i]);
	}
}

void checkOpenGLExtension(string roz) {
	if (!glewIsSupported(roz.c_str())) {
		Logger::log(Logger::ERR + "unhandled extension " + roz);
	}
}

double getLength2D(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double getLength2D(double* p1, double* p2) {
	return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2));
}

double getLength3D(double* p1, double* p2) {
	return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2) + pow(p1[2] - p2[2], 2));
}

void end() {
	TerminateThread(informThread, 0);
	TerminateThread(sortThread, 0);
	ObjectsLoader::getInstance()->terminate();
}

int main(int argc, char** args) {
	atexit(end);
	listsMutex = CreateMutexA(NULL, false, NULL);
	Logger::log("Creating window...");
	glutInit(&argc, args);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Wyœwietlacz");
	glutSetCursor(GLUT_CURSOR_NONE);
	Logger::log("GLEW init...");
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
		Logger::log(Logger::ERR + "OpenGL version " + (char *) glGetString(GL_VERSION) + "against 4.2.0");
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
		Logger::log(Logger::ERR + "SDL init");
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
		Logger::log(Logger::ERR + ", no arguments");
		exit(0);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	culler = FrustumCuller::getInstance();
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
//todo poprawic wyswietlanie zwykle i przy edycji
//todo stl_tree.h:1638
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
