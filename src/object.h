class Row {
public:
	GLfloat* data;

	Row(GLfloat *data) {
		this->data = data;
	}
	~Row() {
		delete[] data;
	}

	GLfloat operator [](const unsigned pos) {
		return data[pos];
	}
};

class Object {
public:
	vector<Subobject*> subobjects;
	MaterialLib *mtl;
	static vector<GLuint> buff;
	static GLuint numerkowybuforXD; //todo
	string name;
	GLfloat min[3][3];
	GLfloat max[3][3];
	int counter;
	int tmpmtl;

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
	void takietamwczytywanie(string nazwa, bool tag) {
		Logger::log("Obiekt: " + nazwa);
		long long unsigned rozmiarpliku;
		long long unsigned ktorywierzcholek = 0;
		long long unsigned ktorynormalny = 0;
		long long unsigned ktoratekstura = 0;
		long long unsigned ktorywierzcholek2 = 0;
		long long unsigned ktorynormalny2 = 0;
		long long unsigned ktoratekstura2 = 0;
		string napis;
		bool silnik_od_tostera = true;
		ifstream wczytywacz;
		wczytywacz.open(nazwa.c_str(), ios::binary);
		if (!wczytywacz.is_open()) {
			Logger::log(Logger::ERR + "brak .obj");
			exit(0);
		}
		wczytywacz.seekg(0, ios::end);
		rozmiarpliku = wczytywacz.tellg();
		wczytywacz.seekg(0, ios::beg);
		ostringstream stream;
		stream << "Rozmiar pliku " << rozmiarpliku << "B";
		Logger::log(stream.str());

		vector<GLfloat> vertices;
		vector<GLfloat> normals;
		vector<GLfloat> textureCords;
		vector<Row*> faces;

		while (!wczytywacz.eof() && napis != "mtllib") {
			wczytywacz >> napis;
		}
		wczytywacz >> napis;
		//xd << this->nazwa << endl;
		materialy[ilematerialow] = new MaterialLib(utnij(this->name) + "/" + napis);
		mtl = materialy[ilematerialow++];

		while (!wczytywacz.eof()) {
			wczytywacz >> napis;
			if (napis == "o") {
				if (subobjects.size() == 0)
					bindObject(vertices, normals, textureCords, faces, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
				wczytywacz >> napis;
				silnik_od_tostera = true;
			}

			if (napis == "usemtl") {
				wczytywacz >> napis;
				if (!silnik_od_tostera) {
					if (subobjects.size() == 0)
						bindObject(vertices, normals, textureCords, faces, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
				}
				silnik_od_tostera = false;
				tmpmtl = mtl->searchMaterial(napis);

			}

			if (napis == "s") {
				wczytywacz >> napis;
				if (napis == "1")
					mtl->mtl[mtl->whichMaterial]->s = GL_SMOOTH;
				else
					mtl->mtl[mtl->whichMaterial]->s = GL_FLAT;
			}

			if (napis == "v") {

				wczytywacz >> napis;
				vertices.push_back(atof(napis.c_str()));
				wczytywacz >> napis;
				vertices.push_back(atof(napis.c_str()));
				wczytywacz >> napis;
				vertices.push_back(atof(napis.c_str()));
				ktorywierzcholek+=3;
				napis = "";
			}

			if (napis == "vt") {
				wczytywacz >> napis;
				textureCords.push_back(atof(napis.c_str()));
				wczytywacz >> napis;
				textureCords.push_back(1 - atof(napis.c_str()));
				napis = "";
				ktoratekstura+=2;
			}

			if (napis == "vn") {
				wczytywacz >> napis;
				normals.push_back(atof(napis.c_str()));
				wczytywacz >> napis;
				normals.push_back(atof(napis.c_str()));
				wczytywacz >> napis;
				normals.push_back(atof(napis.c_str()));
				napis = "";
				ktorynormalny+=3;
			}

			if (napis == "f") {
				int tmp;
				for (int i = 0; i < 3; i++) {
					GLfloat* tab = new GLfloat[3];
					wczytywacz >> napis;
					tmp = getNextFaceNumber(napis) - 1;
					tab[0] = tmp;
					tmp = getNextFaceNumber(napis) - 1;
					tab[1] = tmp;
					tmp = getNextFaceNumber(napis) - 1;
					tab[2] = tmp;
					faces.push_back(new Row(tab));
				}
			}
		}
		int lol = 0;
		bindObject(vertices, normals, textureCords, faces, ktorywierzcholek2, ktorynormalny2, ktoratekstura2);
		for (unsigned i = 0; i < subobjects.size(); i++) {
			lol += subobjects[i]->vertexCount;
		}

		lol /= 3;
		ilee += lol;
		minimax(vertices, ktorywierzcholek);

		stream.str("");
		Logger::log(Logger::LINE);
		stream << "Wczytano " << subobjects.size() << " podobiektow";
		Logger::log(stream.str());
		stream.str("");
		stream << "Utworzono " << lol << " trojkatow\n\n";
		Logger::log(stream.str());
	}

	void bindObject(vector<GLfloat> vertices, vector<GLfloat> normals, vector<GLfloat> textureCords, vector<Row*> faces,
			 long long unsigned &ktorywierzcholek2, long long unsigned &ktorynormalny2,
			long long unsigned &ktoratekstura2) {
		vector<GLfloat> normalne;
		vector<GLfloat> wierzcholki;
		vector < GLfloat > tekstury;
		for (long long unsigned i = 0; i < faces.size(); i++) {
			wierzcholki.push_back(vertices[faces[i][0][0] * 3]);
			wierzcholki.push_back(vertices[faces[i][0][0] * 3 + 1]);
			wierzcholki.push_back(vertices[faces[i][0][0] * 3 + 2]);

			ktorywierzcholek2 += 3;
			normalne.push_back(normals[faces[i][0][2] * 3]);
			normalne.push_back(normals[faces[i][0][2] * 3 + 1]);
			normalne.push_back(normals[faces[i][0][2] * 3 + 2]);
			ktorynormalny2 += 3;

			tekstury.push_back(textureCords[faces[i][0][1] * 2]);
			tekstury.push_back(textureCords[faces[i][0][1] * 2 + 1]);
			ktoratekstura2 += 2;
		}

		sendToBuffer(normalne, wierzcholki, tekstury, faces.size());
		subobjects.push_back(new Subobject(faces.size(), tmpmtl, mtl, buff.size() - 3));
		ktorynormalny2 = 0;
		ktoratekstura2 = 0;
		ktorywierzcholek2 = 0;
	}

	void sendToBuffer(vector<GLfloat> normalne, vector<GLfloat> wierzcholki, vector<GLfloat> tekstury, int facesSize) {
		//ilebuforow chujowe
		unsigned size = buff.size();
		GLuint buffers[3];
		glGenBuffers(3, &buffers[0]);
		buff.push_back(buffers[0]);
		buff.push_back(buffers[1]);
		buff.push_back(buffers[2]);

		glBindBuffer(GL_ARRAY_BUFFER, buff[size]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &wierzcholki[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[size + 1]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 3 * sizeof(GLfloat), &normalne[0], GL_STATIC_READ);
		glBindBuffer(GL_ARRAY_BUFFER, buff[size + 2]);
		glBufferData(GL_ARRAY_BUFFER, facesSize * 2 * sizeof(GLfloat), &tekstury[0], GL_STATIC_READ);
	}

	void ustaw(GLfloat t[3], vector<GLfloat> w, int a) {
		t[0] = w[a];
		t[1] = w[a + 1];
		t[2] = w[a + 2];
	}

	void minimax(vector<GLfloat> w, int ktorywierzcholek) {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				min[i][j] = numeric_limits < GLfloat > ::infinity();
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				max[i][j] = -numeric_limits < GLfloat > ::infinity();
		for (int i = 0; i < ktorywierzcholek; i += 3) {
			if (w[i] > max[0][0])
				ustaw(max[0], w, i);
			if (w[i + 1] > max[1][1])
				ustaw(max[1], w, i);
			if (w[i + 2] > max[2][2])
				ustaw(max[2], w, i);
			if (w[i] < min[0][0])
				ustaw(min[0], w, i);
			if (w[i + 1] < min[1][1])
				ustaw(min[1], w, i);
			if (w[i + 2] < min[2][2])
				ustaw(min[2], w, i);
		}
	}

	Object(string nazwa, bool tag = false) {
		this->name = otworz(nazwa, ".obj");
		tmpmtl = 0;
		takietamwczytywanie(this->name, tag);
		this->counter = 0;
	}
	~Object() {
		for (unsigned i = 0; i < subobjects.size(); i++)
			delete subobjects[i];
		delete mtl;
	}
};

vector<GLuint> Object::buff;
