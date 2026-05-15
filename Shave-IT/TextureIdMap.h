#pragma once
#include <unordered_map>
#include "iostream"

class TextureObject;

class TextureIdMap {
public:
	TextureIdMap();

	unsigned int GetId(std::shared_ptr<TextureObject> texture);

private:
	std::unordered_map<std::shared_ptr<TextureObject>, unsigned int> m_textureToId;
	unsigned int m_next;
};