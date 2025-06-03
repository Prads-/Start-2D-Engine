#ifndef PRIORITISED_ASSET_H
#define PRIORITISED_ASSET_H

#include "FileLoader.h"
#include "BoundingBox.h"
#include "Image.h"

struct PrioritisedAsset {
	FileLoader::LevelAsset::Clone asset;
	BoundingBox bBox;
	const Image *sprite;

	PrioritisedAsset(const Image *sprite, FileLoader::LevelAsset::Clone asset, BoundingBox bBox) {
		this->sprite = sprite;
		this->asset = asset;
		this->bBox = bBox;
	}
};

#endif