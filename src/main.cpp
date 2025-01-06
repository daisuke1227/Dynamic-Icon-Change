#include <random>
#include <string>
#include <cstdlib>
#include <iostream>

#include <Geode/Geode.hpp>

#include <Geode/loader/SettingV3.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using got = GameObjectType;

std::random_device rd;
std::mt19937 gen(rd());

bool enableMod = false;  // Enable = enter level, Disable = exit level
bool globalEnableMod = !Mod::get()->getSettingValue<bool>("disable-mod");  // Turns the mod completely off or on

int originalIcons[8] = {1, 1, 1, 1, 1, 1, 1, 1};

int iconIdMinRange[8] = {1, 1, 1, 1, 1, 1, 1, 1};
int iconIdMaxRange[8] = {1, 1, 1, 1, 1, 1, 1, 1};
std::string_view nameRangeSettings[16] = {
	"min-range-cube", "max-range-cube", "min-range-ship", "max-range-ship", 
	"min-range-ball", "max-range-ball", "min-range-ufo", "max-range-ufo", 
	"min-range-wave", "max-range-wave", "min-range-robot", "max-range-robot",
	"min-range-spider", "max-range-spider", "min-range-swing", "max-range-swing",
};

auto gmPtr = GameManager::get();

$execute {
	listenForSettingChanges("disable-mod", [](bool value) {
		globalEnableMod = !value;
	});
}

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
		if (globalEnableMod) {
			enableMod = true;

			for (int i = 0; i <= 7; i++) {
				// init min-max range icons
				int indexMin = i; int indexMax = i;

				if (indexMin != 0) indexMin = indexMin * 2;
				indexMax = indexMin + 1;

				iconIdMinRange[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMin]);
				iconIdMaxRange[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMax]);
				
				// save original icon kit
				originalIcons[i] = gmPtr->activeIconForType(static_cast<IconType>(i));
				log::debug("Original icon №{} - {}", i, originalIcons[i]);
			}

			log::debug("!!! Mod is enable !!!");

		}

		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void onQuit() {
		if (globalEnableMod) {
			enableMod = false;
			resetOriginalIcons();

			log::debug("!!! Mod is disable !!!");
		}

        PlayLayer::onQuit();
    }

};

class $modify(MyPlayerObject, PlayerObject) {

	void toggleFlyMode(bool p0, bool p1) {
		if (enableMod) {
			for (int i = 0; i <= 7; i++) {
				int randIcon = (gen() % (iconIdMaxRange[i] - iconIdMinRange[i] + 1)) + iconIdMinRange[i];
				setIcon(i+1, randIcon);

				if (i == 0)
					PlayerObject::updatePlayerFrame(randIcon);
				else if (i == 5)
					PlayerObject::updatePlayerRobotFrame(randIcon);
				else if (i == 6)
					PlayerObject::updatePlayerSpiderFrame(randIcon);
			}
		}

		PlayerObject::toggleFlyMode(p0, p1);
	}
};

class $modify(MyAppDelegate, AppDelegate) {
	void trySaveGame(bool p0) {
		/*
			loads original icons if game is 
			switched off (when mod ends incorrectly).
		*/

		if (enableMod) 
			resetOriginalIcons();

		AppDelegate::trySaveGame(p0);
	}
};
