#define GLEW_STATIC
#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0
#define coileklatek 10
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
using namespace std;
#include "logger.h"
////////////////////////////////////////////
void DrawString(double x, double y, double z, string string);
string otworz(string nazwa, string koniec);
string utnij(string dupa);
void zapisz();
long long unsigned sprawdz_rozmiar(string nazwa);
class Animation;
class Object;
class Subobject;
class Entity;
class Material;
class MaterialLib;
class FrustumCuller;
class Texture;
////////////////////////////////////////////
Entity * obiekty_animowane[100];
Entity * wybrany;
tagPOINT *mysz_pozycja;
float modelview[16];
bool czywsp = true;
bool wcisnietyprawy = false;
bool wcisnietylewy = false;
GLfloat cx = 0;
GLfloat cy = 0;
GLfloat cx2 = 0, cy2 = 0, cz2 = 0;
int myk = 0;
int myk2 = 0;
int kamera = 5;
double predkosc = 10;
int licznik = 0;
int ramki = 0;
bool obracamy = false;
GLfloat posX = -5, posY = 5, posZ = 11;
int ileanimacji = 0;
int wys = 700, szer = 1300;
long long unsigned totalVerticesCount = 0;
int ktoreswiatlo = 0;
int ktorapos = 0;
GLfloat kutas[] = { 0.4f, 0.4f, 0.4f, 0.4f };
int ktorykutas = -1;
int ktorykutas2 = 0;
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
GLfloat light_diffuse[] = { 0.5f, 0.5f, 0.5f, 0.5f };
GLfloat light_specular[] = { 0.8f, 0.8f, 0.8f, 0.8f };
GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat high_shininess[] = { 100.0f };

HANDLE hThread;
HANDLE hThread2;
HANDLE hThread3;
struct informacja {
	string x1, y1, z1, fps, speed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
};
informacja info;

#include "texture.h"
#include "material.h"
#include "material_lib.h"
#include "subobject.h"
#include "object.h"
#include "entity.h"
#include "animation.h"
#include "map.h"
#include "frustum_culler.h"
FrustumCuller* culler;

void resize(int width, int height) {
	const float ar = (float) width / (float) height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, culler->bottom, culler->top, culler->neardist, culler->fardist);
	culler->ar = ar;
	glMatrixMode(GL_MODELVIEW);
	wys = height;
	szer = width;
}

void rysuj(Entity *ob) {
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
		if (mtl->tkdt != -1) {
			//	glActiveTexture(GL_TEXTURE0);
			//	glClientActiveTexture( GL_TEXTURE0 );
			glEnable(GL_TEXTURE_2D);
			glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[2]]);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glBindTexture(GL_TEXTURE_2D, Texture::txtid[mtl->tkdt]);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[0]]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, Object::buff[object->buffer[1]]);
		glNormalPointer(GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);
		//glDrawElements(GL_TRIANGLES,ob->ob->subobjects[j]->ilewierzcholkow, GL_UNSIGNED_INT, 0);

		if (mtl->tkdt != -1) {
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}
	glPopMatrix();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (!ramki++)
		licznik = clock();
	if (czywsp) {
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		glLoadIdentity();

		DrawString(-2.32, 1.2, -2.5, "FPS: " + info.fps);
		DrawString(-2.32, 1.15, -2.5, "X: " + info.x1);
		DrawString(-2.32, 1.1, -2.5, "Y: " + info.y1);
		DrawString(-2.32, 1.05, -2.5, "Z: " + info.z1);
		DrawString(-2.32, 1, -2.5, "Speed: " + info.speed);

		DrawString(-2.32, 0.95, -2.5, "Ambient: " + info.amb);
		DrawString(-2.32, 0.9, -2.5, "Diffuse: " + info.diff);
		DrawString(-2.32, 0.85, -2.5, "Specular: " + info.spec);
		DrawString(-2.32, 0.8, -2.5, "Pos: " + info.pos);
		DrawString(-2.32, 0.75, -2.5, "Pos: " + info.poss);
		DrawString(-2.32, 0.7, -2.5, "Wszystkie obiekty: " + info.ileob + "   Wyswietlone obiekty: " + info.ileob2);
		DrawString(-2.32, 0.65, -2.5,
				"Obiekt: " + info.ob2 + "  " + Object::objects[ktorykutas2]->name + "   sztuk: " + info.licznikob);
		if (ktorykutas != -1) {
			DrawString(-2.32, 0.6, -2.5, "Zaznaczony obiekt: " + info.ob + "  " + wybrany->object->name);
			if (Entity::allObjects[ktorykutas]->parent)
				DrawString(-2.32, 0.55, -2.5, "Dziecko obiektu: " + wybrany->parent->object->name);
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glLoadIdentity();
	if (ktorykutas > -1)
		glTranslatef(0, 0, -kamera); //obrot kamery
	glRotatef(cx, 1, 0, 0);
	glRotatef(cy, 0, 1, 0);

	if (ktorykutas == -1)
		glTranslatef(-posX, -posY, -posZ);
	else {
		if (obracamy) {
			wybrany->rx = cx2;
			wybrany->ry = -cy2;
			wybrany->rz = cz2;
		}
		wybrany->px = posX;
		wybrany->py = posY;
		wybrany->pz = posZ;
		GLfloat p1 = posX;
		GLfloat p2 = posY;
		GLfloat p3 = posZ;
		Entity *kutas = wybrany->parent;
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
	for (unsigned i = 0; i < Entity::solidObjectsToDisplay.size(); i++)
		if (Entity::solidObjectsToDisplay[i]) {
			rysuj(Entity::solidObjectsToDisplay[i]);
		}
	glEnable(GL_BLEND);
	for (unsigned i = 0; i < Entity::transparentObjectsToDisplay.size(); i++) {
		if (Entity::transparentObjectsToDisplay[i]) {
			rysuj(Entity::transparentObjectsToDisplay[i]);
		}
	}

	//for(int i=0; i<ileobiektow; i++) rysuj(Entity::allObjects[i]);
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
		if (czywsp)
			czywsp = false;
		else
			czywsp = true;
		break;

	case '=':
		if (predkosc < 10)
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
			if (light_ambient[ktorapos] < 1)
				light_ambient[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			break;
		case 1:
			if (light_diffuse[ktorapos] < 1)
				light_diffuse[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			break;
		case 2:
			if (light_specular[ktorapos] < 1)
				light_specular[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			break;
		case 3:
			light_position[ktorapos] += 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			break;
		}
		break;

	case ';':
		switch (ktoreswiatlo) {
		case 0:
			if (light_ambient[ktorapos] > 0)
				light_ambient[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			break;
		case 1:
			if (light_diffuse[ktorapos] > 0)
				light_diffuse[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			break;
		case 2:
			if (light_specular[ktorapos] > 0)
				light_specular[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			break;
		case 3:
			light_position[ktorapos] -= 0.1;
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			break;
		}
		break;

	case '8':
		Entity::allObjects[ktorykutas]->sx += predkosc;
		Entity::allObjects[ktorykutas]->sy += predkosc;
		Entity::allObjects[ktorykutas]->sz += predkosc;
		if (Entity::allObjects[ktorykutas]->anim) {
			Entity::allObjects[ktorykutas]->anim->startSx += predkosc;
			Entity::allObjects[ktorykutas]->anim->startSy += predkosc;
			Entity::allObjects[ktorykutas]->anim->startSz += predkosc;
		}
		break;

	case '5':
		Entity::allObjects[ktorykutas]->sx -= predkosc;
		Entity::allObjects[ktorykutas]->sy -= predkosc;
		Entity::allObjects[ktorykutas]->sz -= predkosc;
		if (Entity::allObjects[ktorykutas]->anim) {
			Entity::allObjects[ktorykutas]->anim->startSx -= predkosc;
			Entity::allObjects[ktorykutas]->anim->startSy -= predkosc;
			Entity::allObjects[ktorykutas]->anim->startSz -= predkosc;
		}
		break;

	case '4':
		if (ktorykutas > -1) {
			wybrany = Entity::allObjects[--ktorykutas];
			posX = Entity::allObjects[ktorykutas]->px;
			posY = Entity::allObjects[ktorykutas]->py;
			posZ = Entity::allObjects[ktorykutas]->pz;
			cx2 = -Entity::allObjects[ktorykutas]->rx;
			cy2 = -Entity::allObjects[ktorykutas]->ry;
		}
		break;

	case '6':
		if (ktorykutas < (int) Entity::allObjects.size() - 1) {
			wybrany = Entity::allObjects[++ktorykutas];
			posX = Entity::allObjects[ktorykutas]->px;
			posY = Entity::allObjects[ktorykutas]->py;
			posZ = Entity::allObjects[ktorykutas]->pz;
			cx2 = -Entity::allObjects[ktorykutas]->rx;
			cy2 = -Entity::allObjects[ktorykutas]->ry;
		}
		break;

	case '7':
		if (ktorykutas != -1) {
			delete Entity::allObjects[ktorykutas];
			Entity::allObjects[ktorykutas] = Entity::allObjects[Entity::allObjects.size()];
			Entity::allObjects[Entity::allObjects.size()] = NULL;
			ktorykutas = -1;
			wybrany = NULL;
		}
		break;

	case '9':
		Entity::allObjects.push_back(new Entity(Object::objects[ktorykutas2]));
		if (ktorykutas != -1)
			Entity::allObjects[Entity::allObjects.size() - 1]->parent = Entity::allObjects[ktorykutas];
		ktorykutas = Entity::allObjects.size() - 1;
		wybrany = Entity::allObjects[ktorykutas];
		break;

	case '1':
		if (ktorykutas2 > 0)
			ktorykutas2--;
		break;

	case '3':
		if (ktorykutas2 < (int)Object::objects.size() - 1)
			ktorykutas2++;
		break;

	case '*':
		zapisz();
		break;

	}

	glutPostRedisplay();
}

void mysza(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	cx -= (h / 2 - y) / 20.0;
	cy -= (w / 2 - x) / 20.0;
	if (cx > 90)
		cx = 90;
	if (cx < -90)
		cx = -90;
	obracamy = false;
	myk++;
	if (myk >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk = 0;
	}
}

void mysza2(int x, int y) {
	int w, h;
	w = glutGet( GLUT_WINDOW_WIDTH);
	h = glutGet( GLUT_WINDOW_HEIGHT);
	if (wcisnietylewy) {
		cx2 -= (h / 2 - y) / 15.0;
		cz2 -= (w / 2 - x) / 15.0;
	}

	if (wcisnietyprawy)
		cy2 -= (w / 2 - x) / 15.0;
	myk2++;
	obracamy = true;
	if (myk2 >= 3) {
		glutWarpPointer(w / 2, h / 2);
		myk2 = 0;
	}
}

void mysza3(int button, int state, int x, int y) {
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		wcisnietylewy = false;
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		wcisnietylewy = true;
	if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
		wcisnietyprawy = false;
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		wcisnietyprawy = true;
}

void kulko(int button, int dir, int x, int y) {
	if (dir > 0)
		kamera -= predkosc;
	else
		kamera += predkosc;
}

void idle(void) {
	glutPostRedisplay();
}

void wczytaj() {

	int x;
	GLfloat a, b, c, d;
	fstream wczytywacz, wczytywacz2;
	string nazwaobiektu;
	wczytywacz2.open("ustawienia/pliki.txt");
	if (!wczytywacz2.is_open()) {
		Logger::log(Logger::ERR + "brak pliku z plikami");
		exit(0);
	}

	while (!wczytywacz2.eof()) {
		wczytywacz2 >> nazwaobiektu;
		Logger::log(nazwaobiektu);
		Object::objects.push_back(new Object(nazwaobiektu));
	}
	wczytywacz2.close();
	mapa *map = new mapa();
	wczytywacz.open("ustawienia/ustawienia.txt");
	if (!wczytywacz.is_open()) {
		Logger::log(Logger::ERR + "brak pliku z ustawieniami");
		exit(0);
	}

	while (!wczytywacz.eof()) {
		Entity* object;
		wczytywacz >> nazwaobiektu;
		if (nazwaobiektu == "o") {
			wczytywacz >> x;
			if (x >= (int) Object::objects.size()) {
				Logger::log(Logger::ERR + "nie ma tyle obiektow");
				exit(0);
			}
			object = new Entity(Object::objects[x]);
			Entity::allObjects.push_back(object);
		}

		if (nazwaobiektu == "p") {
			wczytywacz >> a >> b >> c;
			d = mapa::calculateHeight(a, b, c);
			//xd << "p " << a << " " << d << " " << c << endl;
			object->setPosition(a, d, c);
		}

		if (nazwaobiektu == "pc") {
			wczytywacz >> a >> b >> c;
			//xd << "pc " << a << " " << b << " " << c << endl;
			object->setPosition(a, b, c);
		}

		if (nazwaobiektu == "s") {
			wczytywacz >> a >> b >> c;
			//xd << "s " << a << " " << b << " " << c << endl;
			object->setScale(a, b, c);
		}

		if (nazwaobiektu == "r") {
			wczytywacz >> a >> b >> c;
			//xd << "r " << a << " " << b << " " << c << endl;
			object->setRotation(a, b, c);
		}

		if (nazwaobiektu == "d") {
			wczytywacz >> x;
			//xd << "d " << x << endl;
			object->parent = Entity::allObjects[Entity::allObjects.size() - 1 + x];
		}

		if (nazwaobiektu == "a") {
			wczytywacz >> nazwaobiektu;
			//xd << "animacja " + nazwaobiektu << endl;
			object->anim = new Animation("modele/" + object->object->name, nazwaobiektu, object);
			obiekty_animowane[ileanimacji++] = object;
		}
		if (nazwaobiektu == "v") {
			object->alwaysDisplay = true;
		}
	}
	Entity* mapObject = new Entity(Object::objects.back());
	Entity::allObjects.push_back(mapObject);
	mapObject->alwaysDisplay = true;
	mapObject->setScale(mapa::stosunekx, mapa::stosuneky, mapa::stosunekz);
	SDL_Quit();

	ostringstream stream;
	Logger::log(Logger::LINE);
	stream << "Utworzono " << totalVerticesCount << " trojkatow";
	Logger::log(stream.str());
	stream.str("");
	stream << "Wczytanych obiektow: " << Object::objects.size() << ", wyswietlonych obiektow:"
			<< Entity::allObjects.size();
	Logger::log(stream.str());

	int ilerez = 0;
	unsigned texturesCount = Texture::textures.size();
	GLboolean *czyrezydentne = new GLboolean[texturesCount];
	glAreTexturesResident(texturesCount, &Texture::txtid[0], czyrezydentne);

	for (unsigned i = 0; i < texturesCount; i++)
		if (czyrezydentne[i]) {
			ilerez++;
		}
	delete[] czyrezydentne;

	stream.str("");
	stream << "Tekstur: " << texturesCount << ", rezydentne: " << ilerez << endl;
	Logger::log(stream.str());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Object::numerkowybuforXD);
}

void zapisz() {
//	fstream zapisywacz;
//	zapisywacz.open("ustawienia/dupa.txt", ios::out);
//	int j;
//	for (int i = 0; i < ileobiektow; i++) {
//		for (j = 0; j < ileobiektow2; j++)
//			if (Entity::allObjects[i]->object == obiekty[j])
//				break;
//		zapisywacz << endl << "o " << j << endl;
//		if (Entity::allObjects[i]->anim) {
//			if (Entity::allObjects[i]->anim->startpx || Entity::allObjects[i]->anim->startpy
//					|| Entity::allObjects[i]->anim->startpz)
//				zapisywacz << "p " << Entity::allObjects[i]->anim->startpx << " "
//						<< Entity::allObjects[i]->anim->startpy << " " << Entity::allObjects[i]->anim->startpz << endl;
//			if (Entity::allObjects[i]->anim->startsx || Entity::allObjects[i]->anim->startsy
//					|| Entity::allObjects[i]->anim->startsz)
//				zapisywacz << "s " << Entity::allObjects[i]->anim->startsx << " "
//						<< Entity::allObjects[i]->anim->startsy << " " << Entity::allObjects[i]->anim->startsz << endl;
//			if (Entity::allObjects[i]->anim->startrx || Entity::allObjects[i]->anim->startry
//					|| Entity::allObjects[i]->anim->startrz)
//				zapisywacz << "r " << Entity::allObjects[i]->anim->startrx << " "
//						<< Entity::allObjects[i]->anim->startry << " " << Entity::allObjects[i]->anim->startrz << endl;
//			zapisywacz << "a" << Entity::allObjects[i]->anim->nazwa << endl;
//		} else {
//			if (Entity::allObjects[i]->px || Entity::allObjects[i]->py || Entity::allObjects[i]->pz)
//				zapisywacz << "p " << Entity::allObjects[i]->px << " " << Entity::allObjects[i]->py << " "
//						<< Entity::allObjects[i]->pz << endl;
//			if (Entity::allObjects[i]->sx || Entity::allObjects[i]->sy || Entity::allObjects[i]->sz)
//				zapisywacz << "s " << Entity::allObjects[i]->sx << " " << Entity::allObjects[i]->sy << " "
//						<< Entity::allObjects[i]->sz << endl;
//			if (Entity::allObjects[i]->rx || Entity::allObjects[i]->ry || Entity::allObjects[i]->rz)
//				zapisywacz << "r " << Entity::allObjects[i]->rx << " " << Entity::allObjects[i]->ry << " "
//						<< Entity::allObjects[i]->rz << endl;
//		}
//
//	}
}

long long unsigned sprawdz_rozmiar(string nazwa) {
	long long unsigned kutas;
	ifstream sprawdzacz;
	sprawdzacz.open(nazwa.c_str(), fstream::binary);
	if (!sprawdzacz.is_open())
		exit(0);
	sprawdzacz.seekg(0, ios::end);
	kutas = sprawdzacz.tellg();
	sprawdzacz.close();
	return kutas;

}

void __cdecl animuj(void *kutas) {
	while (1) {
		if (ktorykutas == -1)
			for (int i = 0; i < ileanimacji; i++)
				obiekty_animowane[i]->anim->animuj(obiekty_animowane[i]);
		Sleep(15);
	}
}

void __cdecl informuj(void *kutas) {
	while (1) {
		stringstream x1, y1, z1, fps, speed, amb, diff, spec, pos, poss, ob, ob2, ileob, ileob2, licznikob;
		x1 << posX;
		y1 << posY;
		z1 << posZ;
		amb << light_ambient[0] << " " << light_ambient[1] << " " << light_ambient[2] << " " << light_ambient[3];
		diff << light_diffuse[0] << " " << light_diffuse[1] << " " << light_diffuse[2] << " " << light_diffuse[3];
		spec << light_specular[0] << " " << light_specular[1] << " " << light_specular[2] << " " << light_specular[3];
		pos << light_position[0] << " " << light_position[1] << " " << light_position[2] << " " << light_position[3];
		poss << "Swiatlo: " << ktoreswiatlo << " Pozycja: " << ktorapos;
		ob << ktorykutas;
		ob2 << ktorykutas2;
		ileob << Entity::allObjects.size();
		ileob2 << Entity::solidObjectsToDisplay.size() + Entity::transparentObjectsToDisplay.size();
		licznikob << Object::objects[ktorykutas2]->counter;
		speed << predkosc;
		if (clock() - licznik >= CLOCKS_PER_SEC) {
			fps << ramki;
			info.fps = fps.str();
			ramki = 0;
		}

		info.x1 = x1.str();
		info.y1 = y1.str();
		info.z1 = z1.str();
		info.speed = speed.str();
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

void __cdecl sortuj(void *dupa) {
	while (1) {
		unsigned objectsCount = Entity::allObjects.size();
		vector<Entity*> transparentObjects;
		vector<Entity*> solidObjects;
		for (unsigned i = 0; i < objectsCount; i++) {
			if (culler->isInViewField(i)) {
				if (Entity::allObjects[i]->object->transparent) {
					transparentObjects.push_back(Entity::allObjects[i]);
				} else {
					solidObjects.push_back(Entity::allObjects[i]);
				}
			}
		}

		sort(transparentObjects.begin(), transparentObjects.end(), Entity::compare);
		Entity::transparentObjectsToDisplay = transparentObjects;
		Entity::solidObjectsToDisplay = solidObjects;
		Sleep(100);
	}
}

string utnij(string kutas) {
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

string otworz(string nazwa, string koniec) {
	string nazwa2 = "";
	string nazwa3 = "";
	int tak = 0;
	if (nazwa[1] == ':') {
		int cycki = 0;
		for (unsigned i = 0; i < nazwa.length(); i++)
			if (nazwa[i] == '/' || nazwa[i] == '\\')
				cycki = i;
		string kutas = "";
		for (unsigned i = cycki; i < nazwa.length(); i++)
			kutas += nazwa[i];
		return nazwa + kutas + koniec;
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

void DrawString(double x, double y, double z, string string) {
	glRasterPos3d(x, y, z);
	for (unsigned i = 0; i < string.length(); i++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, (int) string[i]);
}

void sprawdz_rozszerzenie(string roz) {
	if (!glewIsSupported(roz.c_str()))
		Logger::log(Logger::ERR + "nieobslugiwane roszerzenie " + roz);
}

int main(int argc, char* args[]) {
	Logger::log("Tworzenie okna...");
	glutInit(&argc, args);
	glutInitWindowSize(szer, wys);
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
		stream << "Wersja OpenGL: " << (char*) glGetString( GL_VERSION) << ", OK\n";
		Logger::log(stream.str());
		stream.str("");
	}
	sprawdz_rozszerzenie("GL_ARB_framebuffer_object");
	sprawdz_rozszerzenie("GL_EXT_texture_compression_s3tc");
	sprawdz_rozszerzenie("GL_ARB_vertex_buffer_object");

	mysz_pozycja = new tagPOINT;
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(klawiaturka);
	glutPassiveMotionFunc(mysza);
	glutMotionFunc(mysza2);
	glutMouseFunc(mysza3);
	glutMouseWheelFunc(kulko);
	glutIdleFunc(idle);
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		Logger::log(Logger::ERR + "inicjalizacja SDL");
		exit(0);
	}
	Logger::log(Logger::LINE);
	Logger::log("SDL OK");

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	wczytaj();
	glEnableClientState( GL_VERTEX_ARRAY);
	glEnableClientState( GL_NORMAL_ARRAY);
	culler = FrustumCuller::getInstance();
	hThread = (HANDLE) _beginthread(animuj, 0, NULL);
	hThread2 = (HANDLE) _beginthread(informuj, 0, NULL);
	hThread3 = (HANDLE) _beginthread(sortuj, 0, NULL);

	glutMainLoop();
	return 0;
}
//TODO zapis
//todo przezroczystosc  gore do obiektu || rysowanie samych podobiektow
/*
 x86/zlib1.dll
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
