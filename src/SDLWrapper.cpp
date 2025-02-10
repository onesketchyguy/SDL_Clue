#include "SDLWrapper.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
SDLWrapper* SDLWrapper::instance = nullptr;
std::function<void(const char*)> SDLWrapper::onFileDropped{};

const std::string SDLWrapper::DEFAULT_FONT = "fonts/Deutsch.ttf";

bool Mouse::over(gobl::vec2i pos, gobl::vec2i scale)
{
	pos.x *= SDLWrapper::getRenderScale();
	pos.y *= SDLWrapper::getRenderScale();

	scale.x *= SDLWrapper::getRenderScale();
	scale.y *= SDLWrapper::getRenderScale();

	return (x >= pos.x && x <= pos.x + scale.x 
		&& y >= pos.y && y <= pos.y + scale.y);
}

// MARK: Sprite def
class Renderable
{
private:
	static unsigned short ORDER;
	unsigned short sortOrder = 0;
public:
	Renderable(int x, int y, int w, int h, SDL_Color col = { 255, 255, 255, 255 }, bool solid = true) :
		dest{ x, y, w, h }, color(col), solid(solid), sortOrder(ORDER++) {
	}

	static void ResetOrder() { ORDER = 0; }

	void AssignOrder(int v)
	{
		sortOrder = ORDER + v;
	}

	// Comparator function
	static bool compare(Renderable* a, Renderable* b) {
		return a->sortOrder < b->sortOrder;
	}

	typedef enum { RECT, CIRCLE, LINE, SPRITE, TEXT } RenderableType;

	SDL_Rect dest{};
	SDL_Color color{ 255, 255, 255, 255 };
	bool solid = true;

	virtual int getTypeID() { return RECT; }
};

unsigned short Renderable::ORDER = 0;

class Circle : public Renderable
{
private:
	float radius;
public:
	Circle(int x, int y, float radius, SDL_Color col = { 255, 255, 255, 255 }, bool solid = true) :
		Renderable(x, y, 0, 0, col, solid), radius(radius) {
	}

	const float& getRadius() { return radius; }
	int getTypeID() override { return CIRCLE; }
};

class Line : public Renderable
{
public:
	Line(float x1, float y1, float x2, float y2, SDL_Color col = { 255, 255, 255, 255 }) :
		Renderable(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2), col) {
	}

	int getTypeID() override { return LINE; }
};

class Sprite : public Renderable
{
private:
	std::string pathID;

public:
	const std::string& getID() { return pathID; }
	SDL_Rect src{};

	int getTypeID() override { return SPRITE; }

	Sprite(const std::string& path, int x, int y, int w, int h, int srcx, int srcy, int srcw, int srch, SDL_Color col = { 255, 255, 255, 255 }, bool solid = true) :
		src{ srcx, srcy, srcw, srch }, pathID(path), Renderable(x, y, w, h, col, solid) {
	}
};

class Text : public Renderable
{
private:
	std::string text, font;
	uint8_t fontSize;
public:
	int getTypeID() override { return TEXT; }

	const std::string& getString() { return text; }
	const std::string& getFont() { return font; }
	const uint8_t& getFontSize() { return fontSize; }

	Text(std::string t, int x, int y, SDL_Color col, std::string f, uint8_t fSize) : text(t), font(f), fontSize(fSize), Renderable(x, y, 0, 0, col, false) {}
};

// MARK: This pass
std::vector<Renderable*> renderables{};

SDLWrapper::SDLWrapper(const char* appName, int width, int height, float scale) : screenWidth(width), screenHeight(height), renderScale(scale)
{
	instance = this;

	InitSDL(appName);
	Update();
}

SDLWrapper::~SDLWrapper()
{
	//Deallocate surface
	for (auto& s : textures) SDL_DestroyTexture((SDL_Texture*)s.second);

	//Destroy window
	SDL_DestroyWindow(getType<SDL_Window>("window"));

	for (auto& i : fonts)
	{
		TTF_CloseFont(static_cast<TTF_Font*>(i.second));
	}

	TTF_Quit();

	//Quit SDL subsystems
	SDL_Quit();
}

int SDLWrapper::InitSDL(const char* appName)
{
	SDL_SetMainReady();

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1; // 1: Indicates a failure with SDL
	}
	else
	{
		int windowHints = SDL_WINDOW_SHOWN, rendererFlags = SDL_RENDERER_ACCELERATED;

		window = SDL_CreateWindow(appName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, instance->screenWidth * instance->renderScale, instance->screenHeight * instance->renderScale, windowHints);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return 2; // 2: Indicates a failure with the window
		}

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		renderer = SDL_CreateRenderer(window, -1, rendererFlags);

		if (!renderer)
		{
			printf("Failed to create renderer: %s\n", SDL_GetError());

			SDL_DestroyWindow(window);

			return 3; // 3: Indicates a failure with the renderer
		}
	}

	// Push all our new objects onto the stack
	SetType("renderer", renderer);
	SetType("window", window);

	// Ready graphics stuff now
	IMG_Init(IMG_INIT_PNG); // We don't really need all that fancy pantsy stuff, just PNG

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		return 4; // 4: Indicates a failture with the fonts module
	}

	return 0;
}

bool SDLWrapper::Update()
{
	bool appRunning = true;

	SDL_ShowCursor(getMouse().visible);

	// Updated with gobl input method of bit shifting
	for (int i = 1; i < 5; i++)
	{
		if ((instance->mouse.getRaw(i - 1) & 48) >> 5) instance->mouse.assert(i - 1, 0); // Set inputs that are being released to 0
		if ((instance->mouse.getRaw(i - 1) & 12) >> 3) instance->mouse.assert(i - 1, 3); // Set inputs that are being pressed to held
	}

	for (auto& key : instance->keyboard.getAllRaw())
	{
		if ((key.second & 48) >> 5) key.second = 0; // Set inputs that are being released to 0
		if ((key.second & 12) >> 3) key.second = 3; // Set inputs that are being pressed to held
	}

	instance->mouse.wheel = 0;

	//Handle events on queue
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			std::printf("SDL Quit\n");
			appRunning = false;
			// exit(0);
		}
		if (e.type == SDL_DROPFILE)
		{
			if (onFileDropped) onFileDropped(e.drop.file);
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			instance->mouse.wheel = static_cast<float>(e.wheel.y);
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			SDL_GetMouseState(&instance->mouse.x, &instance->mouse.y);
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			if (instance->mouse.bHeld(e.button.button - 1) == false) instance->mouse.assert(e.button.button - 1, 12);
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			instance->mouse.assert(e.button.button - 1, 48);
		}
		else if (e.type == SDL_KEYDOWN)
		{
			if (instance->keyboard.bHeld(e.key.keysym.sym) == false) instance->keyboard.assert(e.key.keysym.sym, 12);
		}
		else if (e.type == SDL_KEYUP)
		{
			instance->keyboard.assert(e.key.keysym.sym, 48);
		}
	}

	// MARK: Render the screen
	SDL_Renderer* renderer = getType<SDL_Renderer>("renderer");

	if (renderer != nullptr)
	{
		SDL_SetRenderDrawColor(renderer, instance->clearColor.r, instance->clearColor.g, instance->clearColor.b, instance->clearColor.a); // Clear the screen
		SDL_RenderClear(renderer);
	}
	else
	{
		std::cout << "No renderer!" << std::endl;
		return false; // Fail exit
	}

	if (instance->usingSortOrder) std::sort(renderables.begin(), renderables.end(), Renderable::compare);

	// Render all the renderables
	for (auto& o : renderables)
	{
		if (!o || o == nullptr) continue;
		SDL_SetRenderDrawColor(renderer, o->color.r, o->color.g, o->color.b, o->color.a);

		if (o->getTypeID() == Renderable::RECT)
		{
			if (o->solid == true) SDL_RenderFillRect(renderer, &o->dest);
			else SDL_RenderDrawRect(renderer, &o->dest);
			delete o;
		}
		else if (o->getTypeID() == Renderable::CIRCLE)
		{
			// FIXME: Implement this, as it is it is not funcitonal
			Circle* circle = dynamic_cast<Circle*>(o);
			const int32_t diameter = static_cast<int32_t>(circle->getRadius() * 2);
			float x = static_cast<float>(circle->dest.x), y = static_cast<float>(circle->dest.y);

			int num = 0;
			SDL_FPoint points[360];
			for (int i = 0; i < 360; i++)
			{
				points[num++] = SDL_FPoint{ x + float(circle->getRadius() * cos(i)), y + float(circle->getRadius() * sin(i)) };
			}

			SDL_RenderDrawLinesF(renderer, points, num);
			delete circle;
		}
		else if (o->getTypeID() == Renderable::LINE)
		{
			SDL_FPoint points[2] = { { (float)o->dest.x, (float)o->dest.y }, { (float)o->dest.w, (float)o->dest.h } };
			SDL_RenderDrawLinesF(renderer, points, 2);
			delete o;
		}
		else if (o->getTypeID() == Renderable::SPRITE)
		{
			Sprite* sprite = dynamic_cast<Sprite*>(o);
			if (sprite != nullptr && instance->textures[sprite->getID()] != nullptr)
			{
				SDL_Texture* texture = static_cast<SDL_Texture*>(instance->textures[sprite->getID()]);
				SDL_RenderCopy(renderer, texture, (sprite->src.w == 0 ? NULL : &sprite->src), &sprite->dest);
				// We don't need to destroy this texture because it's just a reference to a library of textures
			}
			delete sprite;
		}
		else if (o->getTypeID() == Renderable::TEXT)
		{
			Text* text = dynamic_cast<Text*>(o);
			TTF_Font* font = static_cast<TTF_Font*>(instance->fonts.at(text->getFont()));
			TTF_SetFontSize(font, text->getFontSize());

			SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text->getString().c_str(), text->color);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface); // Cleanup the "surface" object

			SDL_QueryTexture(texture, NULL, NULL, &text->dest.w, &text->dest.h);
			SDL_RenderCopy(renderer, texture, NULL, &text->dest);
			SDL_DestroyTexture(texture); // We DO need to destroy this texture because it's been created this frame
			delete text;
		}

		// Clean up the pointer to the renderable
		//delete o; // For some reason cleaning up here instead of in each individual type leaves memory behind, probably something to do with the data stripping
	}

	renderables.clear();
	Renderable::ResetOrder();

	// Push everything onto the screen
	SDL_RenderPresent(getType<SDL_Renderer>("renderer"));
	SDL_UpdateWindowSurface(getType<SDL_Window>("window"));

	return appRunning;
}

// MARK: Draw routines
int SDLWrapper::LoadSprite(const std::string& path)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", path.c_str());
	SDL_Texture* texture = IMG_LoadTexture(getType<SDL_Renderer>("renderer"), path.c_str());

	if (instance->textures.count(path) <= 0) instance->textures.emplace(path, texture);
	else
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Sprite is replacing an existing item. %s already existed in the database!", path.c_str());
		try
		{
			if (instance->textures[path] != NULL) SDL_DestroyTexture((SDL_Texture*)instance->textures[path]);
		}
		catch (const std::exception& e)
		{
			std::cout << "Failed to delete sprite: " << e.what() << std::endl;
		}
		instance->textures[path] = texture;
	}

	return 0;
}

void SDLWrapper::DrawSprite(const std::string& n, gobl::vec2i pos, SDL_Color col) { DrawSprite(n, pos, gobl::vec2i{ 0, 0 }, gobl::vec2i{ 0, 0 }, gobl::vec2i{ 0, 0 }, col, 0); }
void SDLWrapper::DrawSprite(const std::string& n, gobl::vec2i pos, gobl::vec2i scale, SDL_Color col, short order) { DrawSprite(n, pos, scale, gobl::vec2i{ 0, 0 }, gobl::vec2i{ 0, 0 }, col, order); }
void SDLWrapper::DrawSprite(const std::string& n, gobl::vec2i pos, gobl::vec2i scale, gobl::vec2i srcPos, gobl::vec2i srcScale, SDL_Color col, short order)
{
	if (n.size() < 2) throw std::runtime_error("Cannot draw: Unable to parse empty string!");
	if (instance->textures.count(n) <= 0)
	{
		throw std::runtime_error(("Cannot draw: " + n + " because it has not been loaded!").c_str());;
	}

	if (scale.x == 0 && scale.y == 0) SDL_QueryTexture(static_cast<SDL_Texture*>(instance->textures[n]), NULL, NULL, &scale.x, &scale.y); // Load the default size of the texture

	Sprite* sprite = new Sprite(n, pos.x * instance->renderScale, pos.y * instance->renderScale, scale.x * instance->renderScale, scale.y * instance->renderScale, srcPos.x, srcPos.y, srcScale.x, srcScale.y, col);
	if (order != 0)
	{
		sprite->AssignOrder(order);
		instance->usingSortOrder = true;
	}
	renderables.push_back(sprite);
}

void SDLWrapper::DrawRect(int x, int y, int w, int h, SDL_Color col)
{
	renderables.push_back(new Renderable(x * instance->renderScale, y * instance->renderScale, w * instance->renderScale, h * instance->renderScale, col));
}

void SDLWrapper::OutlineRect(int x, int y, int w, int h, SDL_Color col)
{
	renderables.push_back(new Renderable(x * instance->renderScale, y * instance->renderScale, w * instance->renderScale, h * instance->renderScale, col, false));
}

void SDLWrapper::DrawCircle(int x, int y, float rad, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	renderables.push_back(new Circle(x * instance->renderScale, y * instance->renderScale, rad, { r, g, b, a }));
}

void SDLWrapper::OutlineCircle(int x, int y, float rad, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	renderables.push_back(new Circle(x * instance->renderScale, y * instance->renderScale, rad, { r, g, b, a }, false));
}

void SDLWrapper::DrawString(const std::string& str, gobl::vec2i pos, SDL_Color col, const uint8_t& fontSize, const std::string& font)
{
	if (instance->fonts.count(font) <= 0)
		instance->fonts.emplace(font, TTF_OpenFont(font.c_str(), fontSize));

	renderables.push_back(new Text(str, pos.x * instance->renderScale, pos.y * instance->renderScale, col, font, fontSize * instance->renderScale));
}

void SDLWrapper::DrawLine(gobl::vec2f a, gobl::vec2f b, SDL_Color col)
{
	renderables.push_back(new Line(a.x * instance->renderScale, a.y * instance->renderScale, b.x * instance->renderScale, b.y * instance->renderScale, col));
}

void SDLWrapper::SetClear(const SDL_Color& col)
{
	instance->clearColor = col;
}

float SDLWrapper::deltaTime()
{
	static int start = SDL_GetTicks();
	int cur = SDL_GetTicks();

	float dt = static_cast<float>(cur - start) / 1000.0f;
	start = cur;

	return dt;
}

// MARK: Generic storage device
template <typename T>
T* SDLWrapper::getType(const std::string& n)
{
	if (instance->types.count(n) > 0) return static_cast<T*>(instance->types[n]);
	else return nullptr;
}
void SDLWrapper::SetType(const std::string& n, void* o)
{
	if (instance->types.count(n) <= 0) instance->types.emplace(n, o);
	else instance->types[n] = o;
}