class Animation {
private:
	unsigned steps = 0;
	float emptyArray[3] = { 0, 0, 0 };
public:
	GLfloat startpx, startpy, startpz;
	GLfloat startsx, startsy, startsz;
	GLfloat startrx, startry, startrz;
	vector<float*> p;
	vector<float*> s;
	vector<float*> r;
	bool loop = false;
	unsigned actualStep = 0;
	vector<float> speed;
	int counter;
	string nazwa;
	void setTransition(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		p.push_back(tab);
	}

	void setScaleChange(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		s.push_back(tab);
	}

	void setRotationChange(float a, float b, float c) {
		float* tab = new float[3];
		tab[0] = a;
		tab[1] = b;
		tab[2] = c;
		r.push_back(tab);
	}

	void animuj(Entity *ob) {
		ob->px += p[actualStep][0] * speed[actualStep];
		ob->py += p[actualStep][1] * speed[actualStep];
		ob->pz += p[actualStep][2] * speed[actualStep];
		ob->sx += s[actualStep][0] * speed[actualStep];
		ob->sy += s[actualStep][1] * speed[actualStep];
		ob->sz += s[actualStep][2] * speed[actualStep];
		ob->rx += r[actualStep][0] * speed[actualStep];
		ob->ry += r[actualStep][1] * speed[actualStep];
		ob->rz += r[actualStep][2] * speed[actualStep];
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

	Animation(string sciezka, string nazwa, Entity *ob) {
		for (unsigned i = 0; i < steps; i++)
			for (int j = 0; j < 3; j++) {
				p[i][j] = 0;
				s[i][j] = 0;
				r[i][j] = 0;
			}
		startpx = ob->px;
		startpy = ob->py;
		startpz = ob->pz;
		startsx = ob->sx;
		startsy = ob->sy;
		startsz = ob->sz;
		startrx = ob->rx;
		startry = ob->ry;
		startrz = ob->rz;
		fstream file;
		float a, b, c;
		this->nazwa = nazwa;
		sciezka = "animacje/" + nazwa + ".txt";
		file.open(sciezka.c_str());
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
				steps++;
				if (p.size() != steps) {
					p.push_back(emptyArray);
				}
				if (r.size() != steps) {
					r.push_back(emptyArray);
				}
				if (s.size() != steps) {
					s.push_back(emptyArray);
				}
			}
		}
		steps++;
		counter = 1 / speed[0];
	}

};
