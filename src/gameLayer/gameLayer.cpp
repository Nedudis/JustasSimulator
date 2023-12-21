#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <string>

float getRandomPosition(float min, float max) {
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

struct GameData {
	int highScore;
}gameData;

struct GameInfo {
	glm::vec2 rectPos = { 250, 450 };
	glm::vec2 donutPos = { getRandomPosition(50, 450), 75 };
	glm::vec2 hotdogPos = { getRandomPosition(50, 450), 75 };
}gameInfo;

int points = 0;
std::string scoreMsg = "Score: " + std::to_string(points);
void reloadScoreMsg() {
	scoreMsg = "Score: " + std::to_string(points);
}

std::string highscoreMsg = "Highscore: " + std::to_string(gameData.highScore);
void reloadhighScoreMsg() {
	highscoreMsg = "Highscore: " + std::to_string(gameData.highScore);
}

void teleport_donut() {
	int donut_x_new = getRandomPosition(50, 450);
	gameInfo.donutPos = { donut_x_new, 150 };
	return;
}

void teleport_hotdog() {
	int hotdog_x_new = getRandomPosition(50, 450);
	gameInfo.hotdogPos = { hotdog_x_new, 150 };
	return;
}

bool isCollisionWUserDonut() {
	return (gameInfo.donutPos.x < gameInfo.rectPos.x + 75 &&
		gameInfo.donutPos.x + 75 > gameInfo.rectPos.x &&
		gameInfo.donutPos.y < gameInfo.rectPos.y + 75 &&
		gameInfo.donutPos.y + 75 > gameInfo.rectPos.y);
}

bool isCollisionWUserHotdog() {
	return (gameInfo.hotdogPos.x < gameInfo.rectPos.x + 75 &&
		gameInfo.hotdogPos.x + 75 > gameInfo.rectPos.x &&
		gameInfo.hotdogPos.y < gameInfo.rectPos.y + 75 &&
		gameInfo.hotdogPos.y + 75 > gameInfo.rectPos.y);
}

bool isCollisionWEndDonut() {
	return (
		gameInfo.donutPos.y >= 500
		);
}
bool isCollisionWEndHotdog() {
	return (
		gameInfo.hotdogPos.y >= 500
		);
}

gl2d::Renderer2D renderer;
gl2d::Texture t;
gl2d::Texture donut_texture;
gl2d::Texture hotdog_texture;
gl2d::Texture kebab_texture;
gl2d::Font f;

bool inStart = true;
bool inGame = false;
bool inPause = false;
bool inOver = false;
bool inHelp = false;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	//loading the saved data. Loading an entire structure like this makes savind game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

	t.loadFromFile(RESOURCES_PATH "justas.png", true);
	donut_texture.loadFromFile(RESOURCES_PATH "donut.png", false);
	hotdog_texture.loadFromFile(RESOURCES_PATH "hotdog.png", false);
	kebab_texture.loadFromFile(RESOURCES_PATH "kebab.png", false);
	f.createFromFile(RESOURCES_PATH "roboto_black.ttf");
	reloadhighScoreMsg();

	return true;
}

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);


#pragma endregion

	if (platform::isButtonReleased(platform::Button::Space)) {
		if (inStart == true) {
			points = 0;
			inGame = true;
			inStart = false;
		}
		else if (inOver == true) {
			points = 0;
			reloadScoreMsg();
			inGame = true;
			inOver = false;
		}
		else if (inGame == true) {
			inPause = true;
			inGame = false;
		}
		else if (inPause == true) {
			inPause = false;
			inGame = true;
		}
	}
	if (platform::isButtonReleased(platform::Button::Escape) && inGame == false) {
		if (inPause == true) {
			inPause = false;
			inStart = true;
		}
		else if (inOver == true) {
			inOver = false;
			inStart = true;
		}
		else if (inStart == true) {
			closeGame();
			exit(0);
		}
	}
	if (platform::isButtonReleased(platform::Button::W)) {
		if (inStart == true) {
			inStart = false;
			inHelp = true;
		}
		else if (inHelp == true) {
			inHelp = false;
			inStart = true;
		}
	}

	if (platform::isButtonHeld(platform::Button::Left) && inGame == true)
	{
		gameInfo.rectPos.x -= deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::A) && inGame == true) {
		gameInfo.rectPos.x -= deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::Right) && inGame == true)
	{
		gameInfo.rectPos.x += deltaTime * 100;
	}
	if (platform::isButtonHeld(platform::Button::D) && inGame == true)
	{
		gameInfo.rectPos.x += deltaTime * 100;
	}
	if (isCollisionWUserDonut() == true && inGame == true) {
		points++;
		teleport_donut();
		reloadScoreMsg();
	}
	if (isCollisionWUserHotdog() == true && inGame == true) {
		points++;
		teleport_hotdog();
		reloadScoreMsg();
	}
	if (isCollisionWEndDonut() == true && inGame == true) {
		if (points > gameData.highScore) {
			gameData.highScore = points;
		}
		teleport_donut();
		teleport_hotdog();
		inGame = false;
		inPause = false;
		inOver = true;
		reloadhighScoreMsg();
	}
	if (isCollisionWEndHotdog() == true && inGame == true) {
		if (points > gameData.highScore) {
			gameData.highScore = points;
		}
		teleport_hotdog();
		teleport_donut();
		inGame = false;
		inPause = false;
		inOver = true;
		reloadhighScoreMsg();
	}

	if (inHelp == true) {
		renderer.renderText({ 250, 100 }, "Controls/Help", f, Colors_White, 1);
		renderer.renderText({ 250, 150 }, "<W> -> Main menu", f, Colors_White, 0.5);
		renderer.renderText({ 200, 200 }, "[ A ] - Move left", f, Colors_White, 0.3);
		renderer.renderText({ 200, 230 }, "[ D ] - Move right", f, Colors_White, 0.3);
		renderer.renderText({ 200, 260 }, "[ Space ] - Start/Pause/Unpause game", f, Colors_White, 0.3);
		renderer.renderText({ 200, 290 }, "[ Escape ] - Main menu", f, Colors_White, 0.3);
		renderer.renderText({ 240, 310 }, "**(only from Pause or GameOver screens)", f, Colors_White, 0.2);
	}

	if (inGame == false) {
		if (inPause == true) {
			renderer.renderText({ 250, 100 }, "Pause menu", f, Colors_White, 0.5);
			renderer.renderText({ 250, 200 }, "<Space> -> Unpause", f, Colors_White, 0.75);
			renderer.renderText({ 250, 250 }, "<Escape> -> Main menu", f, Colors_White, 0.75);
			renderer.renderText({ 250, 150 }, scoreMsg.c_str(), f, Colors_White, 0.5);
		}
		else if (inStart == true) {
			renderer.renderText({ 250, 100 }, "Justas Simulator", f, Colors_White, 1);
			renderer.renderRectangle({ 200, 135, 100, 100 }, t);
			renderer.renderText({ 250, 250 }, highscoreMsg.c_str(), f, Colors_White, 0.5);
			renderer.renderText({ 250, 290 }, "<Space> -> Play", f, Colors_Green, 0.5);
			renderer.renderText({ 250, 320 }, "<W> -> Controls", f, Colors_Blue, 0.3);
			renderer.renderText({ 250, 350 }, "<Escape> -> Exit", f, Colors_Red, 0.3);
		}
		else if (inOver == true) {
			renderer.renderText({ 250, 80 }, "You lost!!!", f, Colors_Red, 1);
			renderer.renderText({ 250, 140 }, "<Escape> -> Main menu", f, Colors_White, 0.5);
			renderer.renderText({ 250, 375 }, highscoreMsg.c_str(), f, Colors_White, 0.5);
			renderer.renderText({ 250, 200 }, scoreMsg.c_str(), f, Colors_White, 1);
		}
	}

	if (inGame == true) {
		if (points < 5) {
			renderer.renderRectangle({ gameInfo.donutPos, 100, 100 }, donut_texture);
			gameInfo.donutPos.y += deltaTime * 100;
		}
		else {
			renderer.renderRectangle({ gameInfo.donutPos, 100, 100 }, kebab_texture);
			gameInfo.donutPos.y += deltaTime * 100;
			renderer.renderRectangle({ gameInfo.hotdogPos, 100, 100 }, hotdog_texture);
			gameInfo.hotdogPos.y += deltaTime * 100;
		}
		renderer.clearScreen({ 0.2, 0.3, 0.5, 1 });
		gameInfo.rectPos = glm::clamp(gameInfo.rectPos, glm::vec2{ 0,0 }, glm::vec2{ w - 100,h - 100 });
		renderer.renderRectangle({ gameInfo.rectPos, 100, 100 }, t);
		renderer.renderText({ 100, 25 }, scoreMsg.c_str(), f, Colors_White, 0.5);
		renderer.renderText({ platform::getFrameBufferSizeX() - 150, 25 }, "<Space> -> Pause", f, Colors_White, 0.5);
		renderer.renderLine({ 1, 55 }, { platform::getFrameBufferSizeX() - 1, 55 }, { 0.0, 0.0, 0.2, 1 }, 1);
	}

	renderer.flush();

	/*
	ImGui::Begin("Position");
	ImGui::SliderFloat2("Pos", &gameInfo.rectPos[0], 0, w);
	ImGui::End();
	*/
	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

void closeGame()
{

	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
