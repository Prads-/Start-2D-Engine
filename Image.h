#ifndef IMAGE_H
#define IMAGE_H

#include "declarations.h"
#include <string>
#include <cstring>

class Image {
private:
	int32 width, height;
	uint32 *rgba;

public:
	Image();
	
	Image(const Image &image) {
		width = image.width;
		height = image.height;
		if (image.rgba == 0) {
			rgba = 0;
		} else {
			rgba = new uint32[width * height];
			memcpy(rgba, image.rgba, width * height * sizeof(uint32));
		}
	}

	Image(const std::string *path);
	Image(std::string path);
	~Image();

	Image &operator=(Image image) {
		uint32 *temp = image.rgba;
		image.rgba = this->rgba;
		this->rgba = temp;

		this->width = image.width;
		this->height = image.height;

		return *this;
	}

	int32 getWidth() const;
	int32 getHeight() const;

	void setWidth(int32 width);
	void setHeight(int32 height);

	bool loadImage(const std::string *path);
	bool loadImage(std::string path);

	const uint32 *getImagePtr() const;
	uint32 *getModifiablePtr();

	void createImage(int32 width, int32 height);
	void createShadowImage(uint32 *imgPtrOut, uint8 alpha) const;

	void modifyAlphaComponent(uint8 alpha);
};

#endif
