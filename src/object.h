class Object {
private:
	static HANDLE mutex;
	static HANDLE bufferMutex;
	static vector<Object*> objects;
	static map<string, Object*> objectsMap;
	MaterialLib* mtl;

	int getNextFaceNumber(string& a) {
		int x;
		string tmp = "";
		unsigned i;
		for (i = 0; i < a.size(); i++) {
			tmp += a[i];
			if (a[i] == '/')
				break;
		}
		if (i + 1 < a.size()) {
			a = a.substr(i + 1, a.size());
		}
		x = atoi(tmp.c_str());
		if (x < 0)
			x *= -1;
		return x;
	}

	string prepareString(string str) {
		return "[" + name + "] - " + str;
	}

	void setLastMaterial(Subobject*& subObject, Material* mtl) {
		if (subObject) {
			subObject->mtl = mtl;
			subObject = NULL;
		}
	}
	void loadObject(string objectName, bool tag) {
		Logger::log("Wczytujê obiekt: " + objectName);
		long long unsigned fileSize;
		string text;
		ifstream file;
		file.open(objectName.c_str(), ios::binary);
		if (!file.is_open()) {
			Logger::log(Logger::ERR + "brak .obj");
			exit(0);
		}
		file.seekg(0, ios::end);
		fileSize = file.tellg();
		file.seekg(0, ios::beg);
		ostringstream stream;
		stream << "Rozmiar pliku " << fileSize << "B";
		Logger::log(prepareString(stream.str()));

		vector<GLfloat> vertices;
		vector<GLfloat> normals;
		vector<GLfloat> textureCords;
		vector<GLfloat*> faces;

		while (!file.eof() && text != "mtllib") {
			file >> text;
		}
		file >> text;

		Material* tmpmtl = NULL;
		MaterialLib* mtl = new MaterialLib(getRealPath(text, path));
		MaterialLib::materials.push_back(mtl);
		Subobject* lastSubobject = NULL;
		while (!file.eof()) {
			file >> text;
			if (text == "o") {
				if (faces.size() != 0) {
					setLastMaterial(lastSubobject, tmpmtl);
					lastSubobject = bindObject(vertices, normals, textureCords, faces, tmpmtl);
				}
				file >> text;
			}

			if (text == "usemtl") {
				file >> text;
				if (faces.size() != 0) {
					lastSubobject = bindObject(vertices, normals, textureCords, faces, tmpmtl);
				}
				setLastMaterial(lastSubobject, tmpmtl);
				tmpmtl = mtl->searchMaterial(text);
				transparent |= tmpmtl->isTransparent();
			}

			if (text == "s") {
				file >> text;
				if (text == "1") {
					tmpmtl->s = GL_SMOOTH;
				} else {
					tmpmtl->s = GL_FLAT;
				}
			}

			if (text == "v") {

				file >> text;
				vertices.push_back(atof(text.c_str()));
				file >> text;
				vertices.push_back(atof(text.c_str()));
				file >> text;
				vertices.push_back(atof(text.c_str()));
				text = "";
			}

			if (text == "vt") {
				file >> text;
				textureCords.push_back(atof(text.c_str()));
				file >> text;
				textureCords.push_back(1 - atof(text.c_str()));
				text = "";
			}

			if (text == "vn") {
				file >> text;
				normals.push_back(atof(text.c_str()));
				file >> text;
				normals.push_back(atof(text.c_str()));
				file >> text;
				normals.push_back(atof(text.c_str()));
				text = "";
			}

			if (text == "f") {
				for (int i = 0; i < 3; i++) {
					GLfloat* tab = new GLfloat[3];
					file >> text;
					tab[0] = getNextFaceNumber(text) - 1;
					tab[1] = getNextFaceNumber(text) - 1;
					tab[2] = getNextFaceNumber(text) - 1;
					faces.push_back(tab);
				}
			}
		}

		bindObject(vertices, normals, textureCords, faces, tmpmtl);
		int vertexCount = 0;
		for (unsigned i = 0; i < subobjects.size(); i++) {
			vertexCount += subobjects[i]->vertexCount;
		}

		vertexCount /= 3;
		totalVerticesCount += vertexCount;
		findFurthest(vertices);

		stream.str("");
		Logger::log(Logger::LINE);
		stream << "Wczytano " << subobjects.size() << " podobiektów";
		Logger::log(prepareString(stream.str()));
		stream.str("");
		stream << "Utworzono " << vertexCount << " trojkatów";
		Logger::log(prepareString(stream.str()));
	}

	Subobject* bindObject(vector<GLfloat> vertices, vector<GLfloat> normals, vector<GLfloat> textureCords, vector<GLfloat*> &faces,
			Material* mtl) {
		vector<GLfloat> newNormals;
		vector<GLfloat> newVertices;
		vector<GLfloat> newTextureCords;
		for (long long unsigned i = 0; i < faces.size(); i++) {
			newVertices.push_back(vertices[faces[i][0] * 3]);
			newVertices.push_back(vertices[faces[i][0] * 3 + 1]);
			newVertices.push_back(vertices[faces[i][0] * 3 + 2]);

			newNormals.push_back(normals[faces[i][2] * 3]);
			newNormals.push_back(normals[faces[i][2] * 3 + 1]);
			newNormals.push_back(normals[faces[i][2] * 3 + 2]);

			if (textureCords.size()) {
				newTextureCords.push_back(textureCords[faces[i][1] * 2]);
				newTextureCords.push_back(textureCords[faces[i][1] * 2 + 1]);
			}
		}

		GLuint* buffers = sendToBuffer(newNormals, newVertices, newTextureCords, faces.size());
		Subobject* subObject = new Subobject(faces.size(), mtl, buffers);
		subobjects.push_back(subObject);
		delete buffers;
		faces.clear();
		return subObject;
	}

	GLuint* sendToBuffer(vector<GLfloat> normals, vector<GLfloat> vertices, vector<GLfloat> textureCords, int facesSize) {
		GLuint* buffers = new GLuint[3];
		WaitForSingleObject(bufferMutex, INFINITE);
		glGenBuffers(3, &buffers[0]);
		ReleaseMutex(bufferMutex);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &vertices[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 2 * sizeof(GLfloat), &textureCords[0], GL_STATIC_READ);

		return buffers;
	}

	void findFurthest(vector<GLfloat> vertices) {
		double actualRange = 0;
		unsigned index = 0;
		double temp[3];
		for (unsigned i = 0; i < vertices.size(); i += 3) {
			temp[0] = vertices[i];
			temp[1] = vertices[i + 1];
			temp[2] = vertices[i + 2];
			double length = getLength3D(&temp[0], &empty[0]);
			if (length > actualRange) {
				actualRange = length;
				index = i;
			}
		}
		furthest[0] = vertices[index];
		furthest[1] = vertices[index + 1];
		furthest[2] = vertices[index + 2];

	}

public:
	vector<Subobject*> subobjects;
	string name;
	string path;
	GLfloat furthest[3];
	int counter = 0;
	bool transparent = false;

	static Object* getObject(string key) {
		WaitForSingleObject(mutex, INFINITE);
		Object* result = objectsMap[key];
		ReleaseMutex(mutex);
		return result;
	}

	static Object* getObject(int pos) {
		WaitForSingleObject(mutex, INFINITE);
		Object* result = NULL;
		if (objects.size()) {
			result = objects[pos];
		}
		ReleaseMutex(mutex);
		return result;
	}

	static bool isPresentObject(string name) {
		WaitForSingleObject(mutex, INFINITE);
		bool result = !(objectsMap.find(name) == objectsMap.end());
		ReleaseMutex(mutex);
		return result;
	}

	static void reserveObject(string name) {
		WaitForSingleObject(mutex, INFINITE);
		objectsMap[name] = NULL;
		ReleaseMutex(mutex);
	}

	static void addObject(Object* obj) {
		WaitForSingleObject(mutex, INFINITE);
		objects.push_back(obj);
		objectsMap[obj->name] = obj;
		ReleaseMutex(mutex);
	}

	static int objectsCount() {
		return objects.size();
	}

	Object(string name, bool alwaysDisplay = false) {
		this->name = name;
		this->path = getPath(name);
		loadObject(this->name, alwaysDisplay);
	}
	~Object() {
		for (unsigned i = 0; i < subobjects.size(); i++) {
			delete subobjects[i];
		}
		delete mtl;
	}
};
HANDLE Object::mutex = CreateMutex(NULL, FALSE, NULL);
HANDLE Object::bufferMutex = CreateMutex(NULL, FALSE, NULL);
vector<Object*> Object::objects;
map<string, Object*> Object::objectsMap;
