/*
 * thread_worker.h
 *
 *  Created on: 29 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_THREAD_WORKER_H_
#define SRC_THREAD_WORKER_H_
class ThreadWorker {
private:
	static map<string, void*> results;
	static HANDLE threadsMutex;
	static HANDLE resultsMutex;
	static list<HANDLE> threads;
	static ThreadWorker* instance;
	static int count;
	static int currentCount;
	static HGLRC mainContext;
	static HDC hdc;
	static list<HGLRC> freeContexts;
	static list<HGLRC> busyContexts;

	ThreadWorker() {
		threadsMutex = CreateMutex(NULL, FALSE, NULL);
		resultsMutex = CreateMutex(NULL, FALSE, NULL);
		HWND window = GetActiveWindow();
		hdc = GetDC(window);
		mainContext = wglGetCurrentContext();
	}

	int getThreadsCount() {
		return threads.size();
	}

	static HGLRC startLife() {
		WaitForSingleObject(threadsMutex, INFINITE);
		threads.push_back(GetCurrentThread());
		HGLRC context = freeContexts.front();
		busyContexts.push_front(context);
		ReleaseMutex(threadsMutex);
		wglMakeCurrent(hdc, context);
		//glewInit();
		return context;
	}

	static void endLife(HGLRC loaderContext) {
		wglMakeCurrent(NULL, NULL);

		WaitForSingleObject(threadsMutex, INFINITE);
		busyContexts.remove(loaderContext);
		freeContexts.push_back(loaderContext);
		threads.remove(GetCurrentThread());
		ReleaseMutex(threadsMutex);
	}

	void wait() {
		while (getThreadsCount() == count) {
			Sleep(5);
		}
	}

	static void loadObjectThread(void* arg) {
		HGLRC context = startLife();
		string* name = static_cast<string*>(arg);
		Object* object = new Object(name[0]);

		WaitForSingleObject(resultsMutex, INFINITE);
		results[objectKey(name[0])] = object;
		ReleaseMutex(resultsMutex);
		endLife(context);
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
		this->count = count;
		for (int i = 0; i < count; i++) {
			HGLRC loaderContext = wglCreateContext(hdc);
			if (!wglShareLists(mainContext, loaderContext)) { // Order matters
				Logger::log(Logger::ERR + "nie mo¿na wspó³dzielic display lists.");
			}
			freeContexts.push_front(loaderContext);
		}
	}

	Object* loadObject(string name) {
		wait();
		void* args = static_cast<void*>(&name);
		HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) loadObjectThread, (void*) args, 0, NULL);
		WaitForSingleObject(h, INFINITE);

		Object* result;
		WaitForSingleObject(resultsMutex, INFINITE);
		result = static_cast<Object*>(results[objectKey(name)]);
		ReleaseMutex(resultsMutex);

		return result;
	}

	void finish() {
		wglMakeCurrent(hdc, mainContext);
	}

	~ThreadWorker() {
		CloseHandle(threadsMutex);
		CloseHandle(resultsMutex);
		instance = NULL;
		for (list<HGLRC>::iterator it = freeContexts.begin(); it != freeContexts.end(); ++it) {
			wglDeleteContext(*it);
		}
	}

};
int ThreadWorker::count = 0;
ThreadWorker* ThreadWorker::instance = NULL;
map<string, void*> ThreadWorker::results;
HANDLE ThreadWorker::threadsMutex;
HANDLE ThreadWorker::resultsMutex;
list<HANDLE> ThreadWorker::threads;
HDC ThreadWorker::hdc = NULL;
HGLRC ThreadWorker::mainContext = NULL;
list<HGLRC> ThreadWorker::freeContexts;
list<HGLRC> ThreadWorker::busyContexts;
#endif /* SRC_THREAD_WORKER_H_ */
