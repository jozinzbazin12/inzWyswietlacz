/*
 * Tekstura.h
 *
 *  Created on: 8 sie 2015
 *      Author: Grzegorz
 */

#ifndef SRC_TEXTURE_H_
#define SRC_TEXTURE_H_

class Texture {
private:
	static HANDLE mutex;
	static map<string, Texture*> textures;
	SDL_Surface *txt;
	GLenum format;
	GLenum internalformat;
	string extension;

	GLenum whichFormat() {
		if (extension == ".png")
			switch (txt->format->BytesPerPixel) {
			case 4:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					return GL_BGRA;
				} else {
					return GL_RGBA;
				}
			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					return GL_BGR;
				} else {
					return GL_RGB;
				}
			}
		if (extension == ".tif") {
			return GL_RGBA;
		}
		if (extension == ".tga") {
			switch (txt->format->BytesPerPixel) {
			case 4:
				return GL_BGRA;
			case 3:
				return GL_BGR;
			}
		}

		if (extension == ".bmp") {
			return GL_BGR;
		}
		if (txt->format->BytesPerPixel == 1) {
			return GL_INTENSITY;
		}
		return GL_RGB;
	}

	bool isTransparent() {
		unsigned color;
		if (format == GL_RGBA || format == GL_BGRA) {
			for (int i = 0; i < txt->h; i += 16) {
				for (int j = 0; j < txt->w; j += 16) {
					color = ((unsigned int*) txt->pixels)[i * (txt->pitch / sizeof(unsigned int)) + j];
					color &= 0xFF000000;
					color >>= 24;
					if (color < 255) {
						Logger::log("Przezroczysta       " + textureName);
						return true;
					}
				}
			}
		}
		return false;
	}

public:
	GLuint txtid;
	string textureName;
	bool transparent = false;

	static unsigned getTexturesCount() {
		return textures.size();
	}

	static GLuint* getTextureIds() {
		GLuint* tab = new GLuint[getTexturesCount()];
		map<string, Texture*>::iterator iter;
		unsigned i = 0;
		for (iter = textures.begin(); iter != textures.end(); ++iter) {
			tab[i++] = iter->second->txtid;
		}
		return tab;
	}
	static Texture* getTexture(string name, string type) {
		string key = name + "_" + type;
		Texture* txt = textures[key];
		if (!txt) {
			WaitForSingleObject(mutex, INFINITE);
			txt = new Texture(name, type);
			textures[key] = txt;
			ReleaseMutex(mutex);
		}
		return txt;
	}

	Texture(string nazwa, string tex) {
		Logger::log("--Tekstura: " + nazwa);
		textureName = nazwa;
		extension = nazwa.substr(nazwa.find_last_of("."), nazwa.length());
		txt = IMG_Load(nazwa.c_str());
		if (extension == ".jpg" || extension == ".jpeg" || extension == ".jpe" || extension == ".jif"
				|| extension == ".jfif" || extension == ".jfi")
			extension = ".jpg";
		if (extension == ".bmp" || extension == ".dib")
			extension = ".bmp";
		if (extension == ".tif" || extension == ".tiff")
			extension = "tif";
		if (extension == ".tga" || extension == ".tpic")
			extension = ".tga";

		if (txt == NULL) {
			ostringstream ss;
			Logger::log(Logger::ERR + IMG_GetError());
			exit(0);
		}
		format = whichFormat();
		transparent = isTransparent();
		if (format == GL_RGBA || format == GL_BGRA) {
			if (transparent) {
				internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			} else {
				internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			}
		}
		if (format == GL_RGB || format == GL_BGR) {
			internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		}
		if (format == GL_INTENSITY) {
			internalformat = GL_COMPRESSED_INTENSITY;
		}
		GLuint tab[1];
		glGenTextures(1, &tab[0]);
		glBindTexture(GL_TEXTURE_2D, tab[0]);
		txtid = tab[0];
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, txt->w, txt->h, 0, format, GL_UNSIGNED_BYTE, txt->pixels);
		glGenerateMipmap (GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (tex == "map_Kd") {
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);   //Interpolate RGB with RGB
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, kutas);

			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
		}

		glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -2);
		GLclampf a = 1.0f;
		glPrioritizeTextures(1, &txtid, &a);
		SDL_FreeSurface(txt);

	}

};
HANDLE Texture::mutex = CreateMutex(NULL, FALSE, NULL);

#endif /* SRC_TEXTURE_H_ */
