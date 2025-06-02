#include "build.h"
#include "Image.h"
#include "stbImage.h"
using namespace std;

Image::Image() { 
	rgba = 0; 
}

Image::Image(const std::string *path) {
	rgba = 0;
	loadImage(path);
}

Image::Image(std::string path) {
	rgba = 0;
	loadImage(&path);
}

Image::~Image() {
	if (rgba) {
		stbi_image_free(rgba);
	}
}

int32 Image::getWidth() const {
	return width;
}

int32 Image::getHeight() const {
	return height;
}

void Image::setWidth(int32 width) {
	this->width = width;
}

void Image::setHeight(int32 height) {
	this->height = height;
}

bool Image::loadImage(const std::string *path) {
	if (rgba != 0) return false;

	int n;
	rgba = (uint32*)stbi_load(path->c_str(), &width, &height, &n, 4);
	
#ifdef BUILD_FOR_WINDOWS
	if (rgba) {
		int totalPixels = width * height;
		for (int i = 0; i < totalPixels; ++i) {
			rgba[i] = (rgba[i] & 0xFF00FF00) | ((rgba[i] & 0xFF) << 16) | ((rgba[i] & 0xFF0000) >> 16);
		}
	}
#endif

	return (rgba) ? true : false;
}

bool Image::loadImage(std::string path) {
	return loadImage(&path);
}

const uint32 *Image::getImagePtr() const {
	return rgba;
}

uint32 *Image::getModifiablePtr() {
	return rgba;
}

void Image::createImage(int32 width, int32 height) {
	this->width = width;
	this->height = height;
	rgba = new uint32[width * height];
}

void Image::createShadowImage(uint32 *imgPtrOut, uint8 alpha) const {
	uint32 alpha32 = alpha << 24;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if ((rgba[x + y * width] & 0xFFFFFF) == 0) {
				imgPtrOut[x + y * width] = 0;
			} else {
				imgPtrOut[x + y * width] = alpha32 | 0x010101;
			}
		}
	}
}

void Image::modifyAlphaComponent(uint8 alpha) {
	int size = width * height * 4;
	uint8 * rgba8 = (uint8*)rgba;
	for (int i = 3; i < size; i += 4) {
		rgba8[i] = alpha;
	}
}