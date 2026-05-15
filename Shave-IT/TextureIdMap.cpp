#include "TextureIdMap.h"

TextureIdMap::TextureIdMap()
{
	m_next = 1;
	m_textureToId = std::unordered_map<std::shared_ptr<TextureObject>, unsigned int>();
}

unsigned int TextureIdMap::GetId(std::shared_ptr<TextureObject> texture)
{
	auto it = m_textureToId.find(texture);
	if (it != m_textureToId.end()) return it->second;
	m_textureToId[texture] = m_next;
	return m_next++;
}