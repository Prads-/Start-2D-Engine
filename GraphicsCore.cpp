#include "GraphicsCore.h"
#include <cmath>
#include <cstring>

#ifdef BUILD_FOR_UNIX
GraphicsCore::GraphicsCore(int32 frameWidth, int32 frameHeight, int screenResolutionX, int screenResolutionY) {
    commonInit(frameWidth, frameHeight);
    initOpenGL(screenResolutionX, screenResolutionY);
}
#endif

#ifdef BUILD_FOR_WINDOWS
GraphicsCore::GraphicsCore(HWND *hwnd, int32 frameWidth, int32 frameHeight, int screenResolutionX, int screenResolutionY) {
	commonInit(frameWidth, frameHeight);
	
	d3dDevice = 0;
	texScreen = 0;
	vertexBuffer = 0;
	texHUD = 0;
	hudVertexBuffer = 0;

	initializeDirectX(hwnd, screenResolutionX, screenResolutionY);
}
#endif

void GraphicsCore::commonInit(int32 frameWidth, int32 frameHeight) {
	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	frameSize = frameWidth * frameHeight;
    frameBuffers[0] = new uint32[frameSize];
    frameBuffers[1] = new uint32[frameSize];
    hudBuffer = new uint32[HUD_BUFFER_SIZE];
    activeBuffer = 0;
    currentActiveBuffer = frameBuffers[activeBuffer];
    drawingBuffer = frameBuffers[activeBuffer ^ 1];
	backgroundBuffer = new uint32[frameSize];
	renderHUD = false;

	for (int i = 0; i < 360; ++i) {
		float radian = i * 3.14159265f / 180.0f;
		sinFixedPointTable[i] = sin(radian) * 65536;
		cosFixedPointTable[i] = cos(radian) * 65536;
	}
	xOffset = 0;
	yOffset = 0;
}

void GraphicsCore::maintainAspectRatio(int &screenResolutionX, int &screenResolutionY) {
	if (screenResolutionX > screenResolutionY) {
		int newWidth = ((float)frameWidth / frameHeight) * screenResolutionY;
		if (newWidth > screenResolutionX) {
			screenResolutionY = ((float)frameHeight / frameWidth) * screenResolutionX;
		} else {
			screenResolutionX = newWidth;
		}
	} else {
		int newHeight = ((float)frameHeight / frameWidth) * screenResolutionX;
		if (newHeight > screenResolutionY) {
			screenResolutionX = ((float)frameWidth / frameHeight) * screenResolutionY;
		} else {
			screenResolutionY = newHeight;
		}
	}
}

#ifdef BUILD_FOR_UNIX
void GraphicsCore::initOpenGL(int screenResolutionX, int screenResolutionY) {
	if (glewInit() != GLEW_OK) {
		//TODO: Error Handling
		return;
	}

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &hudTexture);
    glBindTexture(GL_TEXTURE_2D, hudTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
    glViewport(0, 0, screenResolutionX, screenResolutionY);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    halfResolutionX = screenResolutionX / 2.0f;
    halfResolutionY = screenResolutionY / 2.0f;
    glOrtho(-halfResolutionX, halfResolutionX, -halfResolutionY, halfResolutionY, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsCore::openGLCleanUp() {
    glDeleteTextures(1, &frameTexture);
    glDeleteTextures(1, &hudTexture);
}
#endif

#ifdef BUILD_FOR_WINDOWS
bool GraphicsCore::initializeDirectX(HWND *hwnd, int screenResolutionX, int screenResolutionY) {
	this->hwnd = hwnd;
	this->screenResolutionX = screenResolutionX;
	this->screenResolutionY = screenResolutionY;
	deviceIsLost = false;

	IDirect3D9 *d3d;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL) return false;

	D3DCAPS9 d3dCaps;

	if (d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps) != D3D_OK) {
		d3d->Release();
		return false;
	}

	int vP;
	if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vP = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vP = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dInfo = {0};

	d3dInfo.BackBufferCount = 1;
	d3dInfo.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dInfo.BackBufferHeight = screenResolutionY;
	d3dInfo.BackBufferWidth = screenResolutionX;
	d3dInfo.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dInfo.hDeviceWindow = *hwnd;
	d3dInfo.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dInfo.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dInfo.Windowed = TRUE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, *hwnd, vP, &d3dInfo, &d3dDevice) != 0) {
		d3d->Release();
		return false;
	}

	d3d->Release();

	D3DXVECTOR3 cPos(0.0f, 0.0f, -1.419f);
	D3DXVECTOR3 cAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cUp(0.0f, 1.0f, 0.0f);

	D3DXMATRIX cOut;
	D3DXMatrixLookAtLH(&cOut, &cPos, &cAt, &cUp);
	d3dDevice->SetTransform(D3DTS_VIEW, &cOut);

	//Set Projection
	D3DXMATRIX pProj;
	D3DXMatrixOrthoLH(&pProj, screenResolutionX, screenResolutionY, 0.1f, 2.0f);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &pProj);
	
	//Set some states
	d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	
	if (D3DXCreateTexture(d3dDevice, 1024, 1024, 0, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texScreen) != D3D_OK) {
		return false;
	}
	
	if (D3DXCreateTexture(d3dDevice, 128, 128, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texHUD) != D3D_OK) {
		return false;
	}

	if (d3dDevice->CreateVertexBuffer(6 * sizeof(TDVertices), 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &vertexBuffer, NULL) != D3D_OK) {
		return false;
	}
	if (d3dDevice->CreateVertexBuffer(6 * sizeof(TDVertices), 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &hudVertexBuffer, NULL) != D3D_OK) {
		return false;
	}

	maintainAspectRatio(screenResolutionX, screenResolutionY);
	halfResolutionX = screenResolutionX / 2;
	halfResolutionY = screenResolutionY / 2;
	createScreenPlane();
	createHUDPlane(screenResolutionX, screenResolutionY);

	d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return true;
}

void GraphicsCore::createScreenPlane() {
	TDVertices *pVData;
	vertexBuffer->Lock(0, 0, (void**)&pVData, 0);

	pVData[0].x = -halfResolutionX;		pVData[0].y = -halfResolutionY;		pVData[0].z = 0.0f; 
	pVData[0].u = 0.0f;			pVData[0].v = 0.75f;
	
	pVData[1].x = -halfResolutionX;		pVData[1].y = halfResolutionY;			pVData[1].z = 0.0f; 
	pVData[1].u = 0.0f;			pVData[1].v = 0.0f;
	
	pVData[2].x = halfResolutionX;		pVData[2].y = halfResolutionY;			pVData[2].z = 0.0f; 
	pVData[2].u = 1.0f;			pVData[2].v = 0.0f;
	
	pVData[3].x = halfResolutionX;		pVData[3].y = halfResolutionY;			pVData[3].z = 0.0f; 
	pVData[3].u = 1.0f;			pVData[3].v = 0.0f;
	
	pVData[4].x = halfResolutionX;		pVData[4].y = -halfResolutionY;		pVData[4].z = 0.0f; 
	pVData[4].u = 1.0f;			pVData[4].v = 0.75f;
	
	pVData[5].x = -halfResolutionX;		pVData[5].y = -halfResolutionY;		pVData[5].z = 0.0f; 
	pVData[5].u = 0.0f;			pVData[5].v = 0.75f;

	vertexBuffer->Unlock();
}

void GraphicsCore::createHUDPlane(float rX, float rY) {
	rX = ((57.0f * rX) / 1024.0f) / 2.0f;
	rY = ((102.0f * rY) / 768.0f) / 2.0f;

	TDVertices *pVData;
	hudVertexBuffer->Lock(0, 0, (void**)&pVData, 0);

	pVData[0].x = -rX;		pVData[0].y = -rY;		pVData[0].z = 0.0f; 
	pVData[0].u = 0.0f;			pVData[0].v = 0.797f;
	
	pVData[1].x = -rX;		pVData[1].y = rY;			pVData[1].z = 0.0f; 
	pVData[1].u = 0.0f;			pVData[1].v = 0.0f;
	
	pVData[2].x = rX;		pVData[2].y = rY;			pVData[2].z = 0.0f; 
	pVData[2].u = 0.4453f;		pVData[2].v = 0.0f;
	
	pVData[3].x = rX;		pVData[3].y = rY;			pVData[3].z = 0.0f; 
	pVData[3].u = 0.4453f;		pVData[3].v = 0.0f;
	
	pVData[4].x = rX;		pVData[4].y = -rY;		pVData[4].z = 0.0f; 
	pVData[4].u = 0.4453f;		pVData[4].v = 0.797f;
	
	pVData[5].x = -rX;		pVData[5].y = -rY;		pVData[5].z = 0.0f; 
	pVData[5].u = 0.0f;			pVData[5].v = 0.797f;

	hudVertexBuffer->Unlock();
}

void GraphicsCore::directXCleanUp() {
	if (d3dDevice) {
		d3dDevice->Release();
	}
	if (texScreen) {
		texScreen->Release();
	}
	if (vertexBuffer) {
		vertexBuffer->Release();
	}
	if (texHUD) {
		texHUD->Release();
	}
	if (hudVertexBuffer) {
		hudVertexBuffer->Release();
	}
}

void GraphicsCore::deviceLost() {
	if (d3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		directXCleanUp();
		initializeDirectX(hwnd, screenResolutionX, screenResolutionY);
	}
}
#endif

GraphicsCore::~GraphicsCore() {
	delete [] frameBuffers[0];
    delete [] frameBuffers[1];
	delete [] backgroundBuffer;
	delete [] hudBuffer;

#ifdef BUILD_FOR_UNIX
	openGLCleanUp();
#endif
#ifdef BUILD_FOR_WINDOWS
	directXCleanUp();
#endif
}

void GraphicsCore::setOffsetCoordinates(float x, float y) {
	this->xOffset = x;
	this->yOffset = y;
}

void GraphicsCore::clearFrameBuffer(int bgColor) {
	for (int32 i = 0; i < frameSize; ++i) {
		drawingBuffer[i] = bgColor;
	}
}

void GraphicsCore::clearFrameBuffer() {
	memcpy(drawingBuffer, backgroundBuffer, frameWidth * frameHeight * 4);
}

void GraphicsCore::drawText(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y) {
	if (y < 0) {
		return;
	}
#ifdef BUILD_FOR_UNIX
	fontColor = (fontColor & 0xFF00FF00) | ((fontColor & 0xFF0000) >> 16) | ((fontColor & 0xFF) << 8);
#endif
	size_t textSize = text.size();
	int fontImageWidth = fontPtr->getWidth(), fontImageHeight = fontPtr->getHeight();
	const uint32 *fontImagePtr = fontPtr->getImagePtr();

	for (int i = 0; i < textSize; ++i) {
		if (x < 0) {
			x += fontSpace;
			continue;
		}
		
		char alpha = text[i];
		if (alpha == ' ') {
			x += fontSpace;
			if (x + fontSpace >= frameWidth) {
				return;
			}
			continue;
		}

		alpha -= 32;
		int fontX = (alpha % 8) * fontWidth;
		int fontY = (alpha / 8) * fontHeight;
		int variableY = y, variableX;

		for (int j = 0; j < fontHeight; ++j) {
			variableX = x;
			for (int k = 0; k < fontSpace; ++k) {
				if (fontImagePtr[fontX + k + fontY * fontImageWidth] & 0xFFFFFF) {
					drawingBuffer[variableX + variableY * frameWidth] = fontColor;
				}
				++variableX;
			}
			++variableY;
			if (variableY >= frameHeight) {
				break;
			}
			++fontY;
		}

		x += fontSpace;
		if (x + fontSpace >= frameWidth) {
			return;
		}
	}
}

void GraphicsCore::drawText(std::string text, const VariableSizeFont *variableSizeFont, int fontSpace, int characterPadding, uint32 fontColor, int x, int y) {
	if (y < 0) {
		return;
	}
#ifdef BUILD_FOR_UNIX
	fontColor = (fontColor & 0xFF00FF00) | ((fontColor & 0xFF0000) >> 16) | ((fontColor & 0xFF) << 8);
#endif
	for (int i = 0; i < text.size(); ++i) {
		if (x >= FRAME_WIDTH) {
			break;
		}
		const Image *characterImage = variableSizeFont->getFontImage(text[i]);
		if (characterImage == 0 || x < 0) {
			x += fontSpace;
			continue;
		}
		const uint32 *characterImageRGB = characterImage->getImagePtr();
		int characterWidth = characterImage->getWidth(), characterHeight = characterImage->getHeight();
		if (x + characterWidth >= FRAME_WIDTH) {
			break;
		}
		int tempY = y, tempX = x;
		for (int yCounter = 0; yCounter < characterHeight; ++yCounter, ++tempY) {
			if (tempY >= frameHeight) {
				break;
			}
			for (int xCounter = 0; xCounter < characterWidth; ++xCounter, ++tempX) {
				uint32 srcColor = characterImageRGB[xCounter + yCounter * characterWidth];
				if (srcColor & 0xFFFFFF) {
					uint32 *destColor = &drawingBuffer[tempX + tempY * frameWidth];
					*destColor = alphaBlend((srcColor & 0xFF000000) | fontColor, *destColor);
				}
			}
			tempX = x;
		}
		x += characterWidth + characterPadding;
	}
}

void GraphicsCore::draw(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y) {
	//Checks to see if the image is inside or outside the view
	if (x > frameWidth) return;
	if (y > frameHeight) return;
	if (imagePtr->getWidth() + x < 0) return;
	if (imagePtr->getHeight() + y < 0) return;

	if (rotationAngle != 0)
		drawRotation(imagePtr, rotationAngle, flipX, flipY, alphaBlend, x, y);
	else
		drawNoRotation(imagePtr, rotationAngle, flipX, flipY, alphaBlend, x, y);
}

void GraphicsCore::presentBuffer() {
    currentActiveBuffer = frameBuffers[activeBuffer];
    activeBuffer ^= 1;
    drawingBuffer = frameBuffers[activeBuffer];

#ifdef BUILD_FOR_WINDOWS
    D3DLOCKED_RECT lockedRect;
	texScreen->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD);
	for (int y = 0; y < frameHeight; ++y) {
		/*for (int x = 0; x < frameWidth; ++x) {
			*(unsigned int*)((char*)lockedRect.pBits + x * 4 + y * lockedRect.Pitch) = drawingBuffer[x + y * frameWidth];
		}*/
		memcpy((char*)lockedRect.pBits + y * lockedRect.Pitch, currentActiveBuffer + y * frameWidth, frameWidth * 4);
	}
	texScreen->UnlockRect(0);
#endif

#ifdef BUILD_FOR_UNIX
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frameWidth, frameHeight, GL_RGBA, GL_UNSIGNED_BYTE, currentActiveBuffer);
#endif
}

uint32 *GraphicsCore::getFrameBuffer() {
	return drawingBuffer;
}

uint32 *GraphicsCore::getBackgroundBuffer() {
	return backgroundBuffer;
}

void GraphicsCore::clearBackgroundBuffer(int bgColor) {
	drawingBuffer = backgroundBuffer;
	clearFrameBuffer(bgColor);
    drawingBuffer = frameBuffers[activeBuffer ^ 1];
}

void GraphicsCore::drawBackground(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y) {
    drawingBuffer = backgroundBuffer;
	draw(imagePtr, rotationAngle, flipX, flipY, alphaBlend, x, y);
    drawingBuffer = frameBuffers[activeBuffer ^ 1];
}

uint32 GraphicsCore::alphaBlend(uint32 src, uint32 dest) {
	int srb = src & 0xFF00FF, sg = src & 0xFF00;
	int drb = dest & 0xFF00FF, dg = dest & 0xFF00;
	int sa = ((float)(src >> 24) / 255) * 256;

	drb = (sa * (srb - drb) >> 8) + drb;
	dg = (sa * (sg - dg) >> 8) + dg;

	return 0xFF000000 | (drb & 0xFF00FF) | (dg & 0xFF00);
}

void GraphicsCore::drawRotation(const Image *imgPtr, int angle, bool flipX, bool flipY, bool alpha, int x, int y) {
	int imageWidth = imgPtr->getWidth(), imageHeight = imgPtr->getHeight();
	
	int sinA = sinFixedPointTable[angle % 360], cosA = cosFixedPointTable[angle % 360];
	int diagonalLength = sqrt((float)(imageWidth * imageWidth) + (float)(imageHeight * imageHeight));
	int midLength = diagonalLength >> 1;
	int minX = ~midLength + 1, maxX = midLength, minY = ~midLength + 1, maxY = midLength;
	int fixedPointMidWidth = imageWidth << 15, fixedPointMidHeight = imageHeight << 15; 
	int rX, rY;
	const uint32 *image = imgPtr->getImagePtr();

	//Making sure image will be in right coordinates
	x = x - ((diagonalLength - imageWidth) >> 1);
	y = y - ((diagonalLength - imageHeight) >> 1);

	//Cancel the part that is outside the view - right side of the screen
	if (diagonalLength + x > frameWidth) { 
		int difference = diagonalLength + x - frameWidth;
		(flipX) ? minX += difference : maxX -= difference;
	}
	//Cancel the part that is outside the view - lower side of the screen
	if (diagonalLength + y > frameHeight) {
		int difference = diagonalLength + y - frameHeight;
		(flipY) ? minY += difference : maxY -= difference;
	}

	if (x < 0) {	//If the starting point (x) is outside the view - left side of the screen
		(flipX) ? maxX += x : minX -= x;	//Cancel the part outside view
		x = 0;
	}
	if (y < 0) {	//If the starting point (y) is outside the view - upper side of the screen
		(flipY) ? maxY += y : minY -= y;	//Cancel the part outside view
		y = 0;
	}

	//If there are flips, point to the data at the end
	if (flipX) {
		x = x + maxX - minX - 1;
	}
	if (flipY) {
		y = y + maxY - minY - 1;
	}

	//Loop that draws to frame buffer
	if (alpha) {
		for (int imgY = minY; imgY < maxY; ++imgY) {
			for (int imgX = minX, fx = x; imgX < maxX; ++imgX) {
				rX = (imgX * cosA + imgY * sinA + fixedPointMidWidth) >> 16;
				rY = (imgY * cosA - imgX * sinA + fixedPointMidHeight) >> 16;
				if (rX >= 0 && rX < imageWidth && rY >= 0 && rY < imageHeight) {
					int pixel = image[rX + imageWidth * rY];
					if (pixel & 0xFFFFFF) {
						drawingBuffer[fx + frameWidth * y] = alphaBlend(image[rX + imageWidth * rY], drawingBuffer[fx + frameHeight * y]);
					}
				}
				(flipX) ? --fx : ++fx;
			}
			(flipY) ? --y : ++y;
		}
	} else {
		for (int imgY = minY; imgY < maxY; ++imgY) {
			for (int imgX = minX, fx = x; imgX < maxX; ++imgX) {
				rX = (imgX * cosA + imgY * sinA + fixedPointMidWidth) >> 16;
				rY = (imgY * cosA - imgX * sinA + fixedPointMidHeight) >> 16;
				if (rX >= 0 && rX < imageWidth && rY >= 0 && rY < imageHeight) {
					int pixel = image[rX + imageWidth * rY];
					if (pixel & 0xFFFFFF) {
						drawingBuffer[fx + frameWidth * y] = image[rX + imageWidth * rY];
					}
				}
				(flipX) ? --fx : ++fx;
			}
			(flipY) ? --y : ++y;
		}
	}
}

void GraphicsCore::drawNoRotation(const Image *imgPtr, int angle, bool flipX, bool flipY, bool alpha, int x, int y) {
	int fx, imageWidth = imgPtr->getWidth(), imageHeight = imgPtr->getHeight();
	int minX = 0, maxX = imageWidth, minY = 0, maxY = imageHeight;
	const uint32 *image = imgPtr->getImagePtr();

	if (x + imageWidth > frameWidth) {
		int difference = x + imageWidth - frameWidth;
		(flipX) ? minX += difference : maxX -= difference;
	}
	if (x < 0) {
		(flipX) ? maxX += x : minX -= x;
		x = 0;
	}

	if (y + imageHeight > frameHeight) {
		int difference = y + imageHeight - frameHeight;
		(flipY) ? minY += difference : maxY -= difference;
	}
	if (y < 0) {
		(flipY) ? maxY += y : minY -= y;
		y = 0;
	}

	if (flipX) {
		x = x + maxX - minX - 1;
	}
	if (flipY) {
		y = y + maxY - minY - 1;
	}

	if (alpha) {
		for (int imgY = minY; imgY < maxY; ++imgY) {
			y *= frameWidth;
			fx = x;
			for (int imgX = minX; imgX < maxX; ++imgX) {
				uint32 pixel = image[imgX + imageWidth * imgY];
				if (pixel & 0xFFFFFF) {
					drawingBuffer[fx + y] = alphaBlend(image[imgX + imageWidth * imgY], drawingBuffer[fx + y]);
				}
				(flipX) ? --fx : ++fx;
			}
			y /= frameWidth;
			(flipY) ? --y : ++y;
		}
	} else {
		for (int imgY = minY; imgY < maxY; ++imgY) {
			y *= frameWidth;
			fx = x;
			for (int imgX = minX; imgX < maxX; ++imgX) {
				uint32 pixel = image[imgX + imageWidth * imgY];
				if (pixel & 0xFFFFFF) {
					drawingBuffer[fx + y] = image[imgX + imageWidth * imgY];
				}
				(flipX) ? --fx : ++fx;
			}
			y /= frameWidth;
			(flipY) ? --y : ++y;
		}
	}
}

void GraphicsCore::render() {
#ifdef BUILD_FOR_UNIX
	glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
	glTranslatef(xOffset, yOffset, 0.0f);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.000488f, 0.000488f);
    glVertex2f(-halfResolutionX, halfResolutionY);
	glTexCoord2f(1.000488f, 0.000488f);
    glVertex2f(halfResolutionX, halfResolutionY);
	glTexCoord2f(1.000488f, 0.750488f);
    glVertex2f(halfResolutionX, -halfResolutionY);
	glTexCoord2f(0.000488f, 0.750488f);
    glVertex2f(-halfResolutionX, -halfResolutionY);
    glEnd();

	if (renderHUD) {
		glEnable(GL_BLEND);
		glTranslatef(36.5f - halfResolutionX, halfResolutionY - 59.0f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, hudTexture);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0039f, 0.0039f);
		glVertex2f(-28.0f, 51.0f);
		glTexCoord2f(0.4414f, 0.0039f);
		glVertex2f(28.0f, 51.0f);
		glTexCoord2f(0.4414f, 0.8008f);
		glVertex2f(28.0f, -51.0f);
		glTexCoord2f(0.0039f, 0.8008f);
		glVertex2f(-28.0f, -51.0f);
		glEnd();
		glDisable(GL_BLEND);
	}

    glFlush();
#endif

#ifdef BUILD_FOR_WINDOWS
	if (deviceIsLost) {
		deviceLost();
	} else {
		d3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
		d3dDevice->BeginScene();

		d3dDevice->SetTexture(0, texScreen);
		d3dDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(TDVertices));
	
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		D3DXMatrixTranslation(&m, xOffset, yOffset, 0.0f);
		d3dDevice->SetTransform(D3DTS_WORLD, &m);
		d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		if (renderHUD) {
			d3dDevice->SetTexture(0, texHUD);
			d3dDevice->SetStreamSource(0, hudVertexBuffer, 0, sizeof(TDVertices));
			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			
			D3DXMatrixTranslation(&m, 36.5f * halfResolutionX / 512.0f - halfResolutionX, halfResolutionY - 59.0f * halfResolutionY / 384.0f, 0.0f);
			d3dDevice->SetTransform(D3DTS_WORLD, &m);
			d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		d3dDevice->EndScene();
		if (d3dDevice->Present(0, 0, 0, 0) == D3DERR_DEVICELOST) {
			deviceIsLost = true;
		}
	}
#endif
}

void GraphicsCore::drawCircle(int x0, int y0, int radius, uint32 color, bool alpha) {
#ifdef BUILD_FOR_UNIX
	color = (color & 0xFF00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0xFF) << 8);
#endif
	int x = radius, y = 0;
	int r2 = radius * radius;
	int errorR;

	while (x >= y) {
		setPixel(x0 + x, y0 + y, color, alpha);
		setPixel(x0 + x, y0 - y, color, alpha);
		setPixel(x0 - x, y0 + y, color, alpha);
		setPixel(x0 - x, y0 - y, color, alpha);
		setPixel(x0 + y, y0 + x, color, alpha);
		setPixel(x0 + y, y0 - x, color, alpha);
		setPixel(x0 - y, y0 + x, color, alpha);
		setPixel(x0 - y, y0 - x, color, alpha);

		++y;
		errorR = x*x + y*y - r2;
		if (errorR > 0) {
			--x;
		}
	}
}

void GraphicsCore::setPixel(int x, int y, uint32 color, bool alpha) {
	if (x >= 0 && x < FRAME_WIDTH && y >= 0 && y < FRAME_HEIGHT) {
		int index = x + y * FRAME_WIDTH;
		drawingBuffer[index] = (alpha) ? alphaBlend(color, drawingBuffer[index]) : color;
	}
}

void GraphicsCore::drawLine(Vector2f *startPoint, Vector2f direction, int length, uint32 color) {
#ifdef BUILD_FOR_UNIX
	color = (color & 0xFF00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0xFF) << 8);
#endif
	for (int i = 0; i < length; ++i) {
		if (startPoint->x < 0 || startPoint->x >= FRAME_WIDTH || startPoint->y < 0 || startPoint->y >= FRAME_HEIGHT) {
			return;
		}
		drawingBuffer[(int)startPoint->x + (int)startPoint->y * FRAME_WIDTH] = color;
		startPoint->x += direction.x;
		startPoint->y += direction.y;
	}
}

void GraphicsCore::drawLineFade(Vector2f startPoint, Vector2f direction, int length, uint32 color) {
#ifdef BUILD_FOR_UNIX
	color = (color & 0xFF00FF00) | ((color & 0xFF0000) >> 16) | ((color & 0xFF) << 8);
#endif
	float alphaValue = 255.0f, alphaSubValue = 256.0f / length;
	for (int i = 0; i < length; ++i) {
		if (startPoint.x < 0 || startPoint.x >= FRAME_WIDTH || startPoint.y < 0 || startPoint.y >= FRAME_HEIGHT) {
			return;
		}
		int index = (int)startPoint.x + (int)startPoint.y * FRAME_WIDTH;
		drawingBuffer[index] = alphaBlend(((int)alphaValue << 24) | color, drawingBuffer[index]);
		startPoint.x += direction.x;
		startPoint.y += direction.y;
		alphaValue -= alphaSubValue;
	}
}

void GraphicsCore::drawThickBeamNoBoundCheck(Vector2f startPoint, Vector2f direction, int length, uint32 (&widthColors)[5]) {
#ifdef BUILD_FOR_UNIX
	for (int i = 0; i < 5; ++i) {
		widthColors[i] = (widthColors[i] & 0xFF00FF00) | ((widthColors[i] & 0xFF0000) >> 16) | ((widthColors[i] & 0xFF) << 8);
	}
#endif

	Vector2f rightVec = Vector2f(direction.y, -direction.x);
	Vector2f currentPoint;
	for (int i = 0; i < length; ++i) {
		currentPoint = startPoint;
		for (int j = 0; j < 5; ++j) {
			int index = (int)currentPoint.x + (int)currentPoint.y * FRAME_WIDTH;
			drawingBuffer[index] = alphaBlend(widthColors[j], drawingBuffer[index]);
			currentPoint.x += rightVec.x;
			currentPoint.y += rightVec.y;
		}
		startPoint.x += direction.x;
		startPoint.y += direction.y;
	}
}

void GraphicsCore::makeFrameMonochrome() {
	for (int i = 0; i < frameSize; ++i) {
		int r, g, b;
		r = (drawingBuffer[i] & 0xFF0000) >> 16;
		g = (drawingBuffer[i] & 0xFF00) >> 8;
		b = drawingBuffer[i] & 0xFF;
		int grey = (r + g + b) / 3;
		drawingBuffer[i] = (grey << 16) | (grey << 8) | grey;
	}
}

void GraphicsCore::makeImageMonochrome(Image *image) {
	uint32 *tempBuffer = drawingBuffer;
	int32 tempFrameSize = frameSize;

	drawingBuffer = image->getModifiablePtr();
	frameSize = image->getWidth() * image->getHeight();
	makeFrameMonochrome();

	drawingBuffer = tempBuffer;
	frameSize = tempFrameSize;
}

void GraphicsCore::clearHUDBuffer(int color) {
	for (int i = 0; i < HUD_BUFFER_SIZE; ++i) {
		hudBuffer[i] = color;
	}
}

void GraphicsCore::drawHUD(const Image *imagePtr, int rotationAngle, bool flipX, bool flipY, bool alphaBlend, int x, int y) {
	int32 tempFrameWidth = frameWidth, tempFrameHeight = frameHeight;
	uint32 *tempDrawingBuffer = drawingBuffer;

	frameWidth = HUD_WIDTH;
	frameHeight = HUD_HEIGHT;
	drawingBuffer = hudBuffer;
	draw(imagePtr, rotationAngle, flipX, flipY, alphaBlend, x, y);

	frameWidth = tempFrameWidth;
	frameHeight = tempFrameHeight;
	drawingBuffer = tempDrawingBuffer;
}

void GraphicsCore::drawTextHUD(std::string text, const Image *fontPtr, int fontWidth, int fontHeight, int fontSpace, uint32 fontColor, int x, int y) {
	int32 tempFrameWidth = frameWidth, tempFrameHeight = frameHeight;
	uint32 *tempDrawingBuffer = drawingBuffer;

	frameWidth = HUD_WIDTH;
	frameHeight = HUD_HEIGHT;
	drawingBuffer = hudBuffer;
	drawText(text, fontPtr, fontWidth, fontHeight, fontSpace, fontColor, x, y);

	frameWidth = tempFrameWidth;
	frameHeight = tempFrameHeight;
	drawingBuffer = tempDrawingBuffer;
}

void GraphicsCore::makeRectAreaDarkerHUD(Vector2i start, Vector2i end, int divisor) {
	for (int y = start.y; y <= end.y; ++y) {
		for (int x = start.x; x <= end.x; ++x) {
			int index = x + y * HUD_WIDTH;
			int a = hudBuffer[index] & 0xFF000000, r = (hudBuffer[index] >> 16) & 0xFF, g = (hudBuffer[index] >> 8) & 0xFF, b = hudBuffer[index] & 0xFF;
			r /= divisor;
			g /= divisor;
			b /= divisor;
			hudBuffer[index] = a | (r << 16) | (g << 8) | b;
		}
	}
}

void GraphicsCore::setRenderHUD(bool render) {
	renderHUD = render;
}

void GraphicsCore::presentHUD() {
#ifdef BUILD_FOR_WINDOWS
	D3DLOCKED_RECT lockedRect;
	texHUD->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD);
	for (int y = 0; y < HUD_HEIGHT; ++y) {
		for (int x = 0; x < HUD_WIDTH; ++x) {
			*(unsigned int*)((char*)lockedRect.pBits + x * 4 + y * lockedRect.Pitch) = hudBuffer[x + y * HUD_WIDTH];
		}
		//memcpy((char*)lockedRect.pBits + y * lockedRect.Pitch, hudBuffer + y * HUD_WIDTH, HUD_WIDTH * 4);
	}
	texHUD->UnlockRect(0);
#endif

#ifdef BUILD_FOR_UNIX
	glBindTexture(GL_TEXTURE_2D, hudTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, HUD_WIDTH, HUD_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, hudBuffer);
#endif
}
