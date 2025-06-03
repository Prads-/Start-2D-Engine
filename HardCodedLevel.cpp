//#include "HardCodedLevel.h"
//#include "stringExtension.h"
//#include "Immobiliser.h"
//using namespace std;
//using namespace StringExtension;
//using namespace FileLoader;
//
//enum Screen {
//	LOGO,
//	TITLE,
//	LOCAL_GAME,
//	NETWORK_GAME_START,
//	NETWORK_GAME,
//	RESULT
//}screen;
//
//HardCodedLevel::HardCodedLevel(StartEngine *engine) : logoTimer(5000), fpsTimer(1000) {
//	this->engine = engine;
//	aiObject = 0;
//	aiObject2 = 0;
//	player = 0;
//	ball = 0;
//	networkGameManager = 0;
//	
//	vector<LevelAsset> assets;
//	try {
//		loadLevelAssetsFile("maps/TestMap/staticAssets.txt", &assets);
//		createBackground(&assets);
//	} catch (exception &e) {
//		//TODO: Handle the errors
//	}
//
//	font.loadImage("Images/font.png");
//	engineLogo.loadImage("Images/start2D.png");
//	marioPicture.loadImage("Images/angryMario.png");
//
//	screen = LOGO;
//	logoTimer.startTimer();
//
//	fpsTimer.startTimer();
//
//	pickupGranade.loadImage("Images/grenade.png");
//	pickupHoming.loadImage("Images/homing.png");
//	pickupImmobiliser.loadImage("Images/immobiliser.png");
//	pickupPinball.loadImage("Images/pinball.png");
//	pickupShield.loadImage("Images/shield.png");
//	pickupPiercingBall.loadImage("Images/piercingBall.png");
//	pickupBattery.loadImage("Images/battery.png");
//	pickupInvisibleBall.loadImage("Images/invisibleBall.png");
//	pickupTeleportBall.loadImage("Images/teleport.png");
//
//	Immobiliser::addAnimationFrame("Images/immobiliser1.png");
//	Immobiliser::addAnimationFrame("Images/immobiliser2.png");
//	Immobiliser::addAnimationFrame("Images/immobiliser3.png");
//	Immobiliser::addAnimationFrame("Images/immobiliser4.png");
//	Immobiliser::addAnimationFrame("Images/immobiliser5.png");
//}
//
//HardCodedLevel::~HardCodedLevel() {
//	delete player;
//	delete ball;
//	delete aiObject;
//	delete aiObject2;
//	delete networkGameManager;
//	pickupItem->cleanUpInstance();
//	explosion->cleanupInstance();
//}
//
//void HardCodedLevel::createBackground(const std::vector<FileLoader::LevelAsset> *levelAssets) {
//	engine->clearBackground(0);
//
//	for (const LevelAsset &levelAsset : *levelAssets) {
//		Image image("maps/TestMap/" + levelAsset.path);
//		for (const LevelAsset::Clone &clone : levelAsset.clones) {
//			engine->drawBackground(&image, clone.rotationAngle, clone.flipX, clone.flipY, clone.alphaBlend, clone.x, clone.y);
//		}
//	}
//}
//
//void HardCodedLevel::renderLevel() {
//	switch (screen) {
//		case LOGO:
//			renderLogo();
//			break;
//		case TITLE:
//			renderTitle();
//			break;
//		case LOCAL_GAME:
//			renderLocalGame();
//			break;
//		case NETWORK_GAME:
//			renderNetworkGame();
//			break;
//		case NETWORK_GAME_START:
//			renderNetworkGameStart();
//			break;
//		case RESULT:
//			renderResult();
//	}
//}
//
//void HardCodedLevel::renderLogo() {
//	engine->clearScreen(0);
//
//	engine->draw(&engineLogo, 0, false, false, false, 0, 0);
//
//	static int fps = 0, showFPS = 0;
//	if (fpsTimer.checkTimer()) {
//		showFPS = fps;
//		fps = 0;
//	} else {
//		++fps;
//	}
//	
//	engine->drawText(intToStr(showFPS), &font, 32, 32, 16, 0xFFFFFFFF, 5, 5);
//
//	engine->presentScreen();
//	
//	if (logoTimer.checkTimer()) {
//		screen = TITLE;
//	}
//}
//
//void HardCodedLevel::renderTitle() {
//	engine->clearScreen(0);
//
//	engine->draw(&marioPicture, 0, false, false, false, 0, 0);
//	engine->drawText("OH NO! MARIO IS ANGRY", &font, 32, 32, 16, 0xFFFFFF, 200, 100);
//	engine->drawText("'CAUSE HE'S NOT IN THIS PROTOTYPE", &font, 32, 32, 16, 0xFFFFFF, 200, 132);
//	engine->drawText("BATTLE BALL - Prototype", &font, 32, 32, 16, 0xFF7F00, 240, 350);
//	engine->drawText("Press 'W' to start Local Game...", &font, 32, 32, 16, 0xFF7F00, 150, 400);
//	engine->drawText("Press 'S' to start Network Game...", &font, 32, 32, 16, 0xFF7F00, 130, 450);
//
//	static int fps = 0, showFPS = 0;
//	if (fpsTimer.checkTimer()) {
//		showFPS = fps;
//		fps = 0;
//	} else {
//		++fps;
//	}
//	
//	engine->drawText(intToStr(showFPS), &font, 32, 32, 16, 0xFFFFFFFF, 5, 5);
//
//	engine->presentScreen();
//
//	if (engine->isKeyPressed(INPUT_KEY_UP)) {
//		startLocalGame();
//		screen = LOCAL_GAME;
//	} else if (engine->isKeyPressed(INPUT_KEY_DOWN)) {
//		startNetworkGame();
//		screen = NETWORK_GAME_START;
//	}
//}
//
//void HardCodedLevel::renderLocalGame() {
//	engine->clearScreen();
//
//	if (!aiObject->isDead()) {
//		aiObject->update();
//		aiObject->draw();
//	}
//
//	if (!aiObject2->isDead()) {
//		aiObject2->update();
//		aiObject2->draw();
//	}
//
//	player->update();
//	player->draw();
//
//	ball->update();
//	ball->draw();
//
//	for (list<Immobiliser>::iterator it = immobilisers.begin(); it != immobilisers.end();) {
//		if (it->getIsActive()) {
//			it->update();
//			it->draw();
//			++it;
//		} else {
//			it = immobilisers.erase(it);
//		}
//	}
//
//	if (pickupItem->getIsActive()) {
//		pickupItem->draw();
//	} else {
//		pickupItem->update();
//	}
//
//	if (explosion->getIsActive()) {
//		explosion->update();
//		explosion->draw();
//	}
//
//	PickupMove pickupMove = player->getPickupMove();
//	switch (pickupMove) {
//		case PICKUP_GRANADE:
//			engine->draw(&pickupGranade, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_HOMING:
//			engine->draw(&pickupHoming, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_IMMOBILISER:
//			engine->draw(&pickupImmobiliser, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_PINBALL:
//			engine->draw(&pickupPinball, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_SHIELD:
//			engine->draw(&pickupShield, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_PIERCING_BALL:
//			engine->draw(&pickupPiercingBall, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_BATTERY:
//			engine->draw(&pickupBattery, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_INVISIBLE_BALL:
//			engine->draw(&pickupInvisibleBall, 0, false, false, false, 5, 50);
//			break;
//		case PICKUP_TELEPORT_BALL:
//			engine->draw(&pickupTeleportBall, 0, false, false, false, 5, 50);
//	}
//
//	static int fps = 0, showFPS = 0;
//	if (fpsTimer.checkTimer()) {
//		showFPS = fps;
//		fps = 0;
//	} else {
//		++fps;
//	}
//	
//	engine->drawText(intToStr(showFPS), &font, 32, 32, 16, 0xFFFFFFFF, 5, 748);
//
//	engine->presentScreen();
//
//	if (player->isDead()) {
//		result = "You Lose!!!";
//		screen = RESULT;
//	} else if (aiObject->isDead() && aiObject2->isDead()) {
//		result = "You Won!!!";
//		screen = RESULT;
//	}
//}
//
//void HardCodedLevel::renderNetworkGame() {
//	engine->clearScreen();
//	networkGameManager->drawAllDumbObjects();
//	engine->presentScreen();
//	networkGameManager->sendInput();
//}
//
//void HardCodedLevel::renderResult() {
//	engine->clearScreen(0);
//	engine->drawText(result, &font, 32, 32, 16, 0x00FFFF, 300, 350);
//	engine->presentScreen();
//}
//
//void HardCodedLevel::startLocalGame() {
//	ball = new BallObject(engine, &bBoxes);
//
//	player = new PlayerObject(engine, ball, &font, &bBoxes, &immobilisers);
//	player->setStartLook(Vector2f(0.0f, -1.0f));
//	
//	bBoxes.push_back(BoundingBox(Vector2i(0, 0), Vector2i(1024, 20)));
//	bBoxes.push_back(BoundingBox(Vector2i(0, 748), Vector2i(1024, 768)));
//	bBoxes.push_back(BoundingBox(Vector2i(0, 0), Vector2i(20, 768)));
//	bBoxes.push_back(BoundingBox(Vector2i(1004, 0), Vector2i(1024, 768)));
//	try {
//		loadLevelBoundingBox("maps/TestMap/bb.txt", &bBoxes);
//	} catch (exception &e) {
//		//TODO: Handle exception
//	}
//
//	AIVariables aiVariables;
//	aiVariables.cautionDistance = 150.0f;
//	aiVariables.maxSpeed = 2.0f;
//	aiVariables.slowTurn = 4.0f;
//	aiVariables.midTurn = 6.0f;
//	aiVariables.sharpTurn = 8.0f;
//	aiVariables.shootingSharpness = 0.087f;
//	aiVariables.height = 40.0f;
//	aiVariables.width = 40.0f;
//	aiObject = new AIObject(engine, ball, &bBoxes, &immobilisers, aiVariables);
//	aiObject->setStartLook(Vector2f(0.0f, -1.0f));
//
//	aiVariables.maxSpeed = 2.5f;
//	aiObject2 = new AIObject(engine, ball, &bBoxes, &immobilisers, aiVariables);
//	aiObject2->setStartLook(Vector2f(0.0f, -1.0f));
//
//	vector<const Competitor*> opponents1;
//	opponents1.push_back(aiObject2);
//	opponents1.push_back(player);
//	aiObject->setOpponents(&opponents1);
//
//	vector<const Competitor*> opponents2;
//	opponents2.push_back(aiObject);
//	opponents2.push_back(player);
//	aiObject2->setOpponents(&opponents2);
//
//	ball->setAtRandomPosition();
//	player->setAtRandomPosition();
//	aiObject->setAtRandomPosition();
//	aiObject2->setAtRandomPosition();
//
//	vector<const Competitor*> competitors;
//	competitors.push_back(player);
//	competitors.push_back(aiObject);
//	competitors.push_back(aiObject2);
//	ball->setCompetitors(competitors);
//
//	pickupItem = PickupItem::getInstance(engine,  &bBoxes);
//	explosion = Explosion::getInstance(engine);
//}
//
//void HardCodedLevel::startNetworkGame() {
//	networkGameManager = new NetworkGameManager(engine);
//	networkGameManager->connectToServer();
//}
//
//void HardCodedLevel::renderNetworkGameStart() {
//	engine->clearScreen(0);
//
//	switch (networkGameManager->getNetworkState()) {
//		case NET_STATE_IN_GAME:
//			screen = NETWORK_GAME;
//			break;
//		case NET_STATE_NETWORK_ERROR:
//			engine->drawText("Network Error", &font, 32, 32, 16, 0xFFFF00, 400, 400);
//			break;
//		case NET_STATE_NOT_CONNECTED:
//			engine->drawText("Connecting to server", &font, 32, 32, 16, 0xFFFF00, 400, 400);
//			break;
//		case NET_STATE_WAITING_FOR_HANDSHAKE:
//			engine->drawText("Waiting for server response", &font, 32, 32, 16, 0xFFFF00, 400, 400);
//			break;
//		case NET_STATE_WAITING_FOR_MATCH_READY_NOTIFICATION:
//			engine->drawText("Waiting for other players", &font, 32, 32, 16, 0xFFFF00, 400, 400);
//			break;
//		case NET_STATE_WAITING_FOR_NAMES:
//			engine->drawText("Almost done", &font, 32, 32, 16, 0xFFFF00, 400, 400);
//	}
//
//	engine->presentScreen();
//}