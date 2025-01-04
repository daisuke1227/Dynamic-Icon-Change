#include <random>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using got = GameObjectType;

std::random_device rd;
std::mt19937 gen(rd());

bool enableMod = false;
int originalIcons[8] = {1, 1, 1, 1, 1, 1, 1, 1};
int iconIdLimits[8] = {28, 7, 2, 3, 2, 1, 1, 43};

auto gmPtr = GameManager::get();

// GJGameLayer::update setplayerframe

void setIcon(int gamemodeId, int iconId) {
	switch (gamemodeId) {
		case 1: gmPtr->setPlayerFrame(iconId);  break;
		case 2: gmPtr->setPlayerShip(iconId);   break;
		case 3: gmPtr->setPlayerBall(iconId);   break;
		case 4: gmPtr->setPlayerBird(iconId);   break;
		case 5: gmPtr->setPlayerDart(iconId);   break;
		case 6: gmPtr->setPlayerRobot(iconId);  break;
		case 7: gmPtr->setPlayerSpider(iconId); break;
		case 8: gmPtr->setPlayerSwing(iconId);  break;

		default: break;
	}
}

void resetOriginalIcons() {
	for (int i = 0; i <= 7; i++) 
		setIcon(i + 1, originalIcons[i]);
}

class $modify(MyPlayLayer, PlayLayer) {

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		enableMod = true;

		for (int i = 0; i <= 7; i++) {
			originalIcons[i] = gmPtr->activeIconForType(static_cast<IconType>(i));
			log::debug("{}", originalIcons[i]);
		}

		log::debug("!!! Mod is enable !!!");
		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void onQuit() {
		enableMod = false;
		resetOriginalIcons();

		log::debug("!!! Mod is disable !!!");
        PlayLayer::onQuit();
    }

};

class $modify(MyPlayerObject, PlayerObject) {

	void toggleFlyMode(bool p0, bool p1) {
		if (enableMod) {
			for (int i = 1; i <= 8; i++) {
				int randIcon = (gen() % iconIdLimits[i - 1]) + 1;
				setIcon(i, randIcon);

				if (i == 1)
					PlayerObject::updatePlayerFrame(randIcon);
				else if (i == 6)
					PlayerObject::updatePlayerRobotFrame(randIcon);
				else if (i == 7)
					PlayerObject::updatePlayerSpiderFrame(randIcon);
				
			}

		}

		PlayerObject::toggleFlyMode(p0, p1);
	}
};
