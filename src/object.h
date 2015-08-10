class Object {
public:
	vector<Subobject*> subobjects;
	MaterialLib *mtl;
	static vector<GLuint> buff;
	static GLuint numerkowybuforXD; //todo
	static vector<Object*> objects;
	string name;
	GLfloat min[3][3];
	GLfloat max[3][3];
	int counter = 0;
	int tmpmtl = 0;
	bool transparent = false;

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
//todo
	void loadObject(string objectName, bool tag) {
		Logger::log("Obiekt: " + objectName);
		long long unsigned fileSize;
		string text;
		bool useMtl = true;
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
		Logger::log(stream.str());

		vector<GLfloat> vertices;
		vector<GLfloat> normals;
		vector<GLfloat> textureCords;
		vector<GLfloat*> faces;

		while (!file.eof() && text != "mtllib") {
			file >> text;
		}
		file >> text;
		//xd << this->nazwa << endl;
		MaterialLib* newLib = new MaterialLib(utnij(this->name) + "/" + text);
		MaterialLib::materials.push_back(newLib);
		mtl = MaterialLib::materials.back();

		bool first = true;
		while (!file.eof()) {
			file >> text;
			if (text == "o") {
				if (first) {
					bindObject(vertices, normals, textureCords, faces);
					faces.clear();
				} else {
					first = false;
				}
				file >> text;
				useMtl = true;
			}

			if (text == "usemtl") {
				file >> text;
				if (!useMtl) {
					bindObject(vertices, normals, textureCords, faces);
					faces.clear();
				}
				useMtl = false;
				tmpmtl = mtl->searchMaterial(text);

			}

			if (text == "s") {
				file >> text;
				if (text == "1")
					mtl->mtl[tmpmtl]->s = GL_SMOOTH;
				else
					mtl->mtl[tmpmtl]->s = GL_FLAT;
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
		int vertexCount = 0;
		bindObject(vertices, normals, textureCords, faces);
		for (unsigned i = 0; i < subobjects.size(); i++) {
			vertexCount += subobjects[i]->vertexCount;
		}

		vertexCount /= 3;
		totalVerticesCount += vertexCount;
		minimax(vertices);

		stream.str("");
		Logger::log(Logger::LINE);
		stream << "Wczytano " << subobjects.size() << " podobiektow";
		Logger::log(stream.str());
		stream.str("");
		stream << "Utworzono " << vertexCount << " trojkatow\n\n";
		Logger::log(stream.str());
	}

	void bindObject(vector<GLfloat> vertices, vector<GLfloat> normals, vector<GLfloat> textureCords,
			vector<GLfloat*> faces) {
		vector<GLfloat> newNormals;
		vector<GLfloat> newVertices;
		vector<GLfloat> newTextureCords;
		for (long long unsigned i = 0; i < faces.size(); i++) {
			//faces[i]-> Row*
			//faces[i][0]->Row
			//faces[i][0][n]->GlFloat[3]
			newVertices.push_back(vertices[faces[i][0] * 3]);
			newVertices.push_back(vertices[faces[i][0] * 3 + 1]);
			newVertices.push_back(vertices[faces[i][0] * 3 + 2]);

			newNormals.push_back(normals[faces[i][2] * 3]);
			newNormals.push_back(normals[faces[i][2] * 3 + 1]);
			newNormals.push_back(normals[faces[i][2] * 3 + 2]);

			newTextureCords.push_back(textureCords[faces[i][1] * 2]);
			newTextureCords.push_back(textureCords[faces[i][1] * 2 + 1]);
		}

		sendToBuffer(newNormals, newVertices, newTextureCords, faces.size());
		subobjects.push_back(new Subobject(faces.size(), mtl->mtl[tmpmtl], buff.size() - 3));
	}

	void sendToBuffer(vector<GLfloat> normals, vector<GLfloat> vertices, vector<GLfloat> textureCords, int facesSize) {
		unsigned size = buff.size();
		GLuint buffers[3];
		glGenBuffers(3, &buffers[0]);
		buff.push_back(buffers[0]);
		buff.push_back(buffers[1]);
		buff.push_back(buffers[2]);

		glBindBuffer(GL_ARRAY_BUFFER, buff[size]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &vertices[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[size + 1]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[size + 2]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 2 * sizeof(GLfloat), &textureCords[0], GL_STATIC_READ);
	}

	void copyElements(GLfloat t[3], vector<GLfloat> w, int a) {
		t[0] = w[a];
		t[1] = w[a + 1];
		t[2] = w[a + 2];
	}

	void minimax(vector<GLfloat> vertices) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				min[i][j] = numeric_limits < GLfloat > ::infinity();
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				max[i][j] = -numeric_limits < GLfloat > ::infinity();
		for (unsigned i = 0; i < vertices.size(); i += 3) {
			if (vertices[i] > max[0][0])
				copyElements(max[0], vertices, i);
			if (vertices[i + 1] > max[1][1])
				copyElements(max[1], vertices, i);
			if (vertices[i + 2] > max[2][2])
				copyElements(max[2], vertices, i);
			if (vertices[i] < min[0][0])
				copyElements(min[0], vertices, i);
			if (vertices[i + 1] < min[1][1])
				copyElements(min[1], vertices, i);
			if (vertices[i + 2] < min[2][2])
				copyElements(min[2], vertices, i);
		}
	}

	Object(string name, bool alwaysDisplay = false) {
		this->name = otworz(name, ".obj");
		loadObject(this->name, alwaysDisplay);
	}
	~Object() {
		for (unsigned i = 0; i < subobjects.size(); i++)
			delete subobjects[i];
		delete mtl;
	}
};
GLuint Object::numerkowybuforXD;
vector<GLuint> Object::buff;
vector<Object*> Object::objects;
