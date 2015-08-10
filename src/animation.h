class Animation {
private:
	unsigned steps = 0;
	float emptyArray[3] = { 0, 0, 0 };
	void newStep() {
		steps++;
		if (position.size() != steps) {
			position.push_back(emptyArray);
		}
		if (rotation.size() != steps) {
			rotation.push_back(emptyArray);
		}
		if (scale.size() != steps) {
			scale.push_back(emptyArray);
		}
	}

public:
	GLfloat startX, startY, startZ;
	GLfloat startSx, startSy, startSz;
	GLfloat startRx, startRy, startRz;
	vector<float*> position;
	vector<float*> scale;
	vector<float*> rotation;
	bool loop = false;
	unsigned actualStep = 0;
	vector<float> speed;
	int counter;
	string name;
	void setTransition(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		position.push_back(tab);
	}

	void setScaleChange(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		scale.push_back(tab);
	}

	void setRotationChange(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		rotation.push_back(tab);
	}

	void animuj(Entity *ob) {
		Logger::log(name);
		ob->px += position[actualStep][0] * speed[actualStep];
		ob->py += position[actualStep][1] * speed[actualStep];
		ob->pz += position[actualStep][2] * speed[actualStep];
		ob->sx += scale[actualStep][0] * speed[actualStep];
		ob->sy += scale[actualStep][1] * speed[actualStep];
		ob->sz += scale[actualStep][2] * speed[actualStep];
		ob->rx += rotation[actualStep][0] * speed[actualStep];
		ob->ry += rotation[actualStep][1] * speed[actualStep];
		ob->rz += rotation[actualStep][2] * speed[actualStep];
		if (ob->rx > 360)
			ob->rx -= 360;
		if (ob->ry > 360)
			ob->ry -= 360;
		if (ob->rz > 360)
			ob->rz -= 360;
		counter--;
		if (counter == 0) {
			actualStep++;
			if (actualStep >= steps) {
				if (loop) {
					actualStep = 0;
					counter = 1 / speed[actualStep];
				} else
					actualStep = -1;
			} else
				counter = 1 / speed[actualStep];
		}

	}

	Animation(string path, string name, Entity *ob) {
		for (unsigned i = 0; i < steps; i++)
			for (int j = 0; j < 3; j++) {
				position[i][j] = 0;
				scale[i][j] = 0;
				rotation[i][j] = 0;
			}
		startX = ob->px;
		startY = ob->py;
		startZ = ob->pz;
		startSx = ob->sx;
		startSy = ob->sy;
		startSz = ob->sz;
		startRx = ob->rx;
		startRy = ob->ry;
		startRz = ob->rz;
		fstream file;
		float a, b, c;
		this->name = name;
		path = "animacje/" + name + ".txt";
		file.open(path.c_str());
		if (!file.is_open()) {
			Logger::log(Logger::ERR + "brak pliku z animacja");
			exit(0);
		}

		string data;
		while (!file.eof()) {
			file >> data;
			if (data == "speed") {
				file >> a;
				speed.push_back(a);
			}

			if (data == "p") {
				file >> a >> b >> c;
				setTransition(a, b, c);
			}

			if (data == "s") {
				file >> a >> b >> c;
				setScaleChange(a, b, c);
			}

			if (data == "r") {
				file >> a >> b >> c;
				setRotationChange(a, b, c);
			}

			if (data == "petla") {
				loop = true;
			}

			if (data == "#") {
				newStep();
			}
		}
		newStep();
		counter = 1 / speed[0];
	}
};
