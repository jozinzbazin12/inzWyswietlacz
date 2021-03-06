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
	static HGLRC mainContext;
	static HDC hdc;
	static Map* mapBuilder;
	static vector<HANDLE> threads;
	static list<xml_node<>*> objectNodes;
	static int workingThreads;
	static xml_node<>* mapNode;

	ThreadWorker() {
		threadsMutex = CreateMutex(NULL, FALSE, NULL);
		HWND window = GetActiveWindow();
		hdc = GetDC(window);
		mainContext = wglGetCurrentContext();
	}

	static void loadEntityThread(xml_node<>* node) {
		if (!node) {
			return;
		}
		GLfloat a, b, c, d;
		string stringValue;
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
			while (!mapBuilder || !mapBuilder->heights) {
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
	}

	static void loadMapThread(xml_node<>* node) {
		if (!node) {
			return;
		}
		GLfloat a, b, c, d;
		string stringValue;
		xml_node<>* settings = node->first_node("Settings");
		stringValue = node->first_attribute("mapFile")->value();
		mapBuilder = new Map();
		a = stod(settings->first_node("lengthX")->value());
		b = stod(settings->first_node("lengthY")->value());
		c = stod(settings->first_node("lengthZ")->value());
		mapBuilder->xLength = a;
		mapBuilder->wymy = b;
		mapBuilder->zLength = c;
		Entity::objects = TreeNode::createRoot(a, c);
		xml_node<>* materialSettings = node->first_node("Material");
		if (materialSettings) {
			MapMaterial* mtl = new MapMaterial();
			xml_node<>* textureSettings = materialSettings->first_node("Texture");
			if (textureSettings) {
				mapBuilder->texturePath = textureSettings->first_attribute("path")->value();
				mapBuilder->scale = stod(textureSettings->first_attribute("scale")->value());
			}
			xml_node<>* mtlSetting = materialSettings->first_node("Ka");
			if (mtlSetting) {
				a = stod(mtlSetting->first_attribute("r")->value());
				b = stod(mtlSetting->first_attribute("g")->value());
				c = stod(mtlSetting->first_attribute("b")->value());
				mtl->setKa(a, b, c);
			}
			mtlSetting = materialSettings->first_node("Kd");
			if (mtlSetting) {
				a = stod(mtlSetting->first_attribute("r")->value());
				b = stod(mtlSetting->first_attribute("g")->value());
				c = stod(mtlSetting->first_attribute("b")->value());
				mtl->setKd(a, b, c);
			}
			mtlSetting = materialSettings->first_node("Ks");
			if (mtlSetting) {
				a = stod(mtlSetting->first_attribute("r")->value());
				b = stod(mtlSetting->first_attribute("g")->value());
				c = stod(mtlSetting->first_attribute("b")->value());
				mtl->setKs(a, b, c);
			}
			mtl->d = stod(materialSettings->first_node("d")->value());
			mtl->ns = stod(materialSettings->first_node("Ns")->value());
			mapBuilder->mtl = mtl;
		}
		xml_node<>* lightSettings = node->first_node("Light");
		if (lightSettings) {
			Light* light = Light::getInstance();
			xml_node<>* type = lightSettings->first_node("Ambient");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				light->setAmbient(a, b, c, d);
			}
			type = lightSettings->first_node("Diffuse");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				light->setDiffuse(a, b, c, d);
			}
			type = lightSettings->first_node("Specular");
			if (type) {
				a = stod(type->first_attribute("r")->value());
				b = stod(type->first_attribute("g")->value());
				c = stod(type->first_attribute("b")->value());
				d = stod(type->first_attribute("a")->value());
				light->setSpecular(a, b, c, d);
			}
			light->setReady(true);
		}

		mapBuilder->createMap(stringValue);
		Object::addObject(mapBuilder->mapObject);
		Entity* mapObject = new Entity(mapBuilder->mapObject);
		mapObject->setScale(mapBuilder->xRate, mapBuilder->yRate, mapBuilder->zRate);
		Entity::addEntity(mapObject);
	}

	static string objectKey(string name) {
		return "object-" + name;
	}

	static void checkErrors() {
		GLenum e;
		while ((e = glGetError())) {
			Logger::log(Logger::ERR + "OpenGL error: " + to_string(e));
		}
	}

	static void jobListener(void* arg) {
		HGLRC context = static_cast<HGLRC>(arg);
		wglMakeCurrent(hdc, context);
		glewInit();
		while (1) {
			xml_node<>* n;
			if (mapNode) {
				WaitForSingleObject(threadsMutex, INFINITE);
				n = mapNode;
				mapNode = NULL;
				workingThreads++;
				ReleaseMutex(threadsMutex);
				loadMapThread(n);
				decCount();
			}
			WaitForSingleObject(threadsMutex, INFINITE);
			if (objectNodes.size()) {
				n = objectNodes.back();
				objectNodes.pop_back();
				workingThreads++;
				ReleaseMutex(threadsMutex);
				loadEntityThread(n);
				decCount();
			}
			ReleaseMutex(threadsMutex);
			Sleep(5);
		}
	}

	static void decCount() {
		WaitForSingleObject(threadsMutex, INFINITE);
		workingThreads--;
		checkErrors();
		ReleaseMutex(threadsMutex);
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
		for (int i = 0; i < count; i++) {
			HGLRC loaderContext = wglCreateContext(hdc);
			if (!wglShareLists(mainContext, loaderContext)) { // Order matters
				Logger::log(Logger::ERR + "cannot share display lists.");
			}
			void* args = static_cast<void*>(loaderContext);
			HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) jobListener, (void*) args, 0, NULL);
			threads.push_back(thread);
			Logger::log("Starting thread " + to_string(GetThreadId(thread)));
		}
	}

	void loadEntity(xml_node<>* node) {
		if (!node || node == (xml_node<>*) 0xfffffffffffffffe) {
			return;
		}
		WaitForSingleObject(threadsMutex, INFINITE);
		objectNodes.push_front(node);
		ReleaseMutex(threadsMutex);
	}

	void loadMap(xml_node<>* node) {
		WaitForSingleObject(threadsMutex, INFINITE);
		mapNode = node;
		ReleaseMutex(threadsMutex);
	}

	void finish() {
		while (objectNodes.size() || workingThreads || mapNode) {
			Sleep(50);
		}
		terminate();
	}

	void terminate() {
		for (unsigned i = 0; i < threads.size(); i++) {
			Logger::log("Thread finished: " + to_string(GetThreadId(threads[i])));
			TerminateThread(threads[i], 0);
		}
	}

	~ThreadWorker() {
		instance = NULL;
//		for (list<HGLRC>::iterator it = freeContexts.begin(); it != freeContexts.end(); ++it) {
//			wglDeleteContext(*it);
//		}
	}

};
xml_node<>* ThreadWorker::mapNode = NULL;
Map* ThreadWorker::mapBuilder = NULL;
ThreadWorker* ThreadWorker::instance = NULL;
HANDLE ThreadWorker::threadsMutex;
HDC ThreadWorker::hdc = NULL;
HGLRC ThreadWorker::mainContext = NULL;
vector<HANDLE> ThreadWorker::threads;
list<xml_node<>*> ThreadWorker::objectNodes;
int ThreadWorker::workingThreads = 0;
#endif /* SRC_THREAD_WORKER_H_ */
