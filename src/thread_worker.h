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
	static list<HGLRC> busyContexts;
	static Map* map;

	ThreadWorker() {
		threadsMutex = CreateMutex(NULL, FALSE, NULL);
		HWND window = GetActiveWindow();
		hdc = GetDC(window);
		mainContext = wglGetCurrentContext();
	}

	static HGLRC startGlLife() {
		WaitForSingleObject(threadsMutex, INFINITE);
		HGLRC context = freeContexts.front();
		busyContexts.push_front(context);
		ReleaseMutex(threadsMutex);
		wglMakeCurrent(hdc, context);
		//glewInit();
		return context;
	}

	static void endGlLife(HGLRC loaderContext) {
		wglMakeCurrent(NULL, NULL);
		WaitForSingleObject(threadsMutex, INFINITE);
		currentCount--;
		busyContexts.remove(loaderContext);
		freeContexts.push_back(loaderContext);
		ReleaseMutex(threadsMutex);
	}

	void wait() {
		while (currentCount >= maxCount) {
			Sleep(5);
		}
	}

	static void loadEntityThread(void* arg) {
		GLfloat a, b, c, d;
		string stringValue;
		xml_node<>* node = static_cast<xml_node<>*>(arg);

		string objectName = node->first_attribute("objectFile")->value();
		Object* object;
		while (!Object::isPresentObject(objectName)) {
			Sleep(10);
		}
		object = Object::getObject(objectName);
		Entity* entity = new Entity(object);
		xml_node<>* settings = node->first_node();
		stringValue = settings->first_node("relative")->value();
		a = stod(settings->first_node("posX")->value());
		b = stod(settings->first_node("posY")->value());
		c = stod(settings->first_node("posZ")->value());
		if (stringValue == "false") {
			entity->setPosition(a, b, c);
		} else {
			d = map->calculateHeight(a, b, c);
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

		currentCount--;
	}

	static void loadObjectThread(void* arg) {
		HGLRC context = startGlLife();
		string *name = static_cast<string*>(arg);
		string nameValue = name[0];

		Object::reserveObject(nameValue);
		Object* object = new Object(nameValue);
		Object::addObject(object);
		delete name;
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

	static void setMap(Map* map_) {
		map = map_;
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
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadEntityThread, (void*) args, 0, NULL);
		currentCount++;
	}

	void loadObject(string name) {
		wait();
		if (!Object::isPresentObject(name)) {
			string* name2 = new string(name);
			void* args = static_cast<void*>(name2);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadObjectThread, (void*) args, 0, NULL);
			currentCount++;
		}
	}

	void finish() {
		while (currentCount) {
			Sleep(10);
		}
		wglMakeCurrent(hdc, mainContext);
	}

	~ThreadWorker() {
		CloseHandle(threadsMutex);
		instance = NULL;
//		for (list<HGLRC>::iterator it = freeContexts.begin(); it != freeContexts.end(); ++it) {
//			wglDeleteContext(*it);
//		}
	}

};

Map* ThreadWorker::map = NULL;
int ThreadWorker::maxCount = 0;
int ThreadWorker::currentCount = 0;
ThreadWorker* ThreadWorker::instance = NULL;
HANDLE ThreadWorker::threadsMutex;
HDC ThreadWorker::hdc = NULL;
HGLRC ThreadWorker::mainContext = NULL;
list<HGLRC> ThreadWorker::freeContexts;
list<HGLRC> ThreadWorker::busyContexts;
#endif /* SRC_THREAD_WORKER_H_ */
