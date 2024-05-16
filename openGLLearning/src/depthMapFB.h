#pragma once

#include "DepthTexture.h"
#include "Renderer.h"


const unsigned int SHADOW_WIDTH = SCR_WIDTH, SHADOW_HEIGHT = SCR_HEIGHT;

class DepthMapFB
{
private:
	unsigned int m_RendererID;

public:
	DepthMapFB();
	void attachTexture(const DepthTexture& tex);
	void bind() const;
	void unBind() const;
	void clear();
};