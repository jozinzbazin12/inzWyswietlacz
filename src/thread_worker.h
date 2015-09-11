/*
 * thread_worker.h
 *
 *  Created on: 29 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_THREAD_WORKER_H_
#define SRC_THREAD_WORKER_H_
using namespace rapidxml;
class ThreadWorker {
private:
	static HANDLE threadsMutex;
	static ThreadWorker* instance;
	static int maxCount;
	static int currentCount;
	static HGLRC mainContext;
	static HDC hdc;
	static list<HGLRC> freeContexts;
	static Map* mapBuilder;

	ThreadWorker() {
		threadsMutex = CreateMutex(NULL, FALSE, NULL);
		HWND window = GetActiveWindow();
		hdc = GetDC(window);
		mainContext = wglGetCurrentContext();
	}

	static HGLRC startGlLife() {
		WaitForSingleObject(threadsMutex, INFINITE);
		HGLRC context = freeContexts.front();
		freeContexts.pop_front();
		ReleaseMutex(threadsMutex);
		wglMakeCurrent(hdc, context);
		return context;
	}

	static void endGlLife(HGLRC loaderContext) {
		wglMakeCurrent(NULL, NULL);
		WaitForSingleObject(threadsMutex, INFINITE);
		currentCount--;
		freeContexts.push_back(loaderContext);
		ReleaseMutex(threadsMutex);
	}

	void wait() {
		while (currentCount >= maxCount) {
			Sleep(5);
		}
	}

	static void loadEntityThread(void* arg) {
		HGLRC context = startGlLife();
		GLfloat a, b, c, d;
		string stringValue;
		xml_node<>* node = static_cast<xml_node<>*>(arg);

		string objectName = node->first_attribute("objectFile")->value();
		Object* object;
		if (Object::isPresentObject(objectName)) {
			while (!(object = Object::getObject(objectName))) {
				Sleep(10);
			}
		} else {
			Object::reserveObject(objectName);
			object = new Object(objectName);
			Object::addObject(object);
		}
		Entity* entity = new Entity(object);
		xml_node<>* settings = node->first_node();
		stringValue = settings->first_node("relative")->value();
		a = stod(settings->first_node("posX")->value());
		b = stod(settings->first_node("posY")->value());
		c = stod(settings->first_node("posZ")->value());
		if (stringValue == "false") {
			entity->setPosition(a, b, c);
		} else {
			while (!mapBuilder) {
				Sleep(10);
			}
			d = mapBuilder->calculateHeight(a, b, c);
			entity->setPosition(a, d, c);
		}

		a = stod(settings->first_node("scaleX")->value());
		b = stod(settings->first_node("scaleY")->value());
		c = stod(settings->first_node("scaleZ")->value());
		entity->setScale(a, b, c);

		a = stod(settings->first_node("rotationX")->value());
		b = stod(settings->first_node("rotationY")->value());
		c = stod(settings->first_node("rotationZ")->value());
		entity->setRotation(a, b, c);
		Entity::addEntity(entity);

		endGlLife(context);
	}

	static void loadMapThread(void* arg) {
		HGLRC context = startGlLife();
		GLfloat a, b, c, d;
		string stringValue;
		xml_node<>* node = static_cast<xml_node<>*>(arg);
		xml_node<>* settings = node->first_node();
		stringValue = node->first_attribute("mapFile")->value();
		mapBuilder = new Map();
		a = stod(settings->first_node("lengthX")->value());
		b = stod(settings->first_node("lengthY")->value());
		c = stod(settings->first_node("lengthZ")->value());
		mapBuilder->wymx = a;
		mapBuilder->wymy = b;
		mapBuilder->wymz = c;
		xml_node<>* lightSettings = node->first_node("Light");
		if (lightSettings) {
			xml_node<>* type = lightSettings->first_node("Ambient");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				Light::getInstance()->setAmbient(a, b, c, d);
			}
			type = lightSettings->first_node("Diffuse");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				Light::getInstance()->setDiffuse(a, b, c, d);
			}
			type = lightSettings->first_node("Specular");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				Light::getInstance()->setSpecular(a, b, c, d);
			}
		}
		Light::getInstance()->setReady(true);
		mapBuilder->createMap(stringValue, "mapy/tekstury/tex.png", "mapy/mtl/mtl.mtl");
		Object::addObject(mapBuilder->mapObject);
		Entity* mapObject = new Entity(mapBuilder->mapObject);
		Entity::addEntity(mapObject);
		mapObject->alwaysDisplay = true;
		mapObject->setScale(mapBuilder->stosunekx, mapBuilder->stosuneky, mapBuilder->stosunekz);
		endGlLife(context);
	}
	static string objectKey(string name) {
		return "object-" + name;
	}

public:
	static ThreadWorker* getInstance() {
		if (!instance) {
			instance = new ThreadWorker();
		}
		return instance;
	}

	void setThreadsCount(int count) {
		if (count < 1) {
			count = 1;
		}
		this->maxCount = count;
		for (int i = 0; i < count; i++) {
			HGLRC loaderContext = wglCreateContext(hdc);
			if (!wglShareLists(mainContext, loaderContext)) { // Order matters
				Logger::log(Logger::ERR + "nie mo¿na wspó³dzielic display lists.");
			}
			freeContexts.push_front(loaderContext);
		}
	}

	void loadEntity(xml_node<>* node) {
		wait();
		void* args = static_cast<void*>(node);
		currentCount++;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadEntityThread, (void*) args, 0, NULL);
	}

	void loadMap(xml_node<>* node) {
		wait();
		void* args = static_cast<void*>(node);
		currentCount++;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadMapThread, (void*) args, 0, NULL);
	}

	void finish() {
		while (currentCount) {
			Sleep(10);
		}
	}

	~ThreadWorker() {
		CloseHandle(threadsMutex);
		instance = NULL;
//		for (list<HGLRC>::iterator it = freeContexts.begin(); it != freeContexts.end(); ++it) {
//			wglDeleteContext(*it);
//		}
	}

};

Map* ThreadWorker::mapBuilder = NULL;
int ThreadWorker::maxCount = 0;
int ThreadWorker::currentCount = 0;
ThreadWorker* ThreadWorker::instance = NULL;
HANDLE ThreadWorker::threadsMutex;
HDC ThreadWorker::hdc = NULL;
HGLRC ThreadWorker::mainContext = NULL;
list<HGLRC> ThreadWorker::freeContexts;
#endif /* SRC_THREAD_WORKER_H_ */
