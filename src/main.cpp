#include <random>
#include <string>
#include <cstdlib>
#include <iostream>

#include <Geode/Geode.hpp>

#include <Geode/loader/SettingV3.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
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

inline int generateRandomIconId(int gmId) {
	return (gen() % (iconIdMaxRange[gmId] - iconIdMinRange[gmId] + 1)) + iconIdMinRange[gmId];
}

void setIcon(
	PlayerObject* po, 
	int gamemodeId, 
	int iconId, 
	bool p0, bool p1,  // player0 - player1
	bool randIcon
) {
	if (!enableMod) return;

	if (!p0 && !p1) {
		if (po->m_isShip || po->m_isBall || po->m_isBird || po->m_isDart || 
		    po->m_isRobot || po->m_isSpider || po->m_isSwing) return;  // if gamemode is not a cube

		gamemodeId = 0;
	}

	if (randIcon) iconId = generateRandomIconId(gamemodeId);

	switch (gamemodeId) {
		case 0: gmPtr->setPlayerFrame(iconId);  break;
		case 1: gmPtr->setPlayerShip(iconId);   break;
		case 2: gmPtr->setPlayerBall(iconId);   break;
		case 3: gmPtr->setPlayerBird(iconId);   break;
		case 4: gmPtr->setPlayerDart(iconId);   break;
		case 5: gmPtr->setPlayerRobot(iconId);  break;
		case 6: gmPtr->setPlayerSpider(iconId); break;
		case 7: gmPtr->setPlayerSwing(iconId);  break;

		default: break;
	}

	if (po == nullptr) return;

	switch (gamemodeId) {
		case 0: po->updatePlayerFrame(iconId);       break;
		case 1: po->updatePlayerShipFrame(iconId);   break;
		case 2: po->updatePlayerRollFrame(iconId);   break;
		case 3: po->updatePlayerBirdFrame(iconId);   break;
		case 4: po->updatePlayerDartFrame(iconId);   break;
		case 5: po->updatePlayerRobotFrame(iconId);  break;
		case 6: po->updatePlayerSpiderFrame(iconId); break;
		case 7: po->updatePlayerSwingFrame(iconId);  break;

		default: break;
	}
}

void saveOriginalIcons() {
	for (int i = 0; i <= 7; i++) {
		log::debug("Save original icons (SAVE)");
		originalIcons[i] = gmPtr->activeIconForType(static_cast<IconType>(i));
		log::debug("Original icon №{} - {}", i, originalIcons[i]);
	}
}

void resetOriginalIcons(PlayerObject* po) {
	log::debug("reset original icons");
	for (int i = 0; i <= 7; i++) {
		setIcon(nullptr, i, originalIcons[i], true, true, false);
		log::debug("{} = {}", i, originalIcons[i]);
	}

	// reset just in time
	if (po == nullptr) return;

	po->updatePlayerFrame(originalIcons[0]);

	if (po->m_isShip)        po->updatePlayerShipFrame(originalIcons[1]);
	else if (po->m_isBall)   po->updatePlayerRollFrame(originalIcons[2]);
	else if (po->m_isBird)   po->updatePlayerBirdFrame(originalIcons[3]);
	else if (po->m_isDart)   po->updatePlayerDartFrame(originalIcons[4]);
	else if (po->m_isRobot)  po->updatePlayerRobotFrame(originalIcons[5]);
	else if (po->m_isSpider) po->updatePlayerSpiderFrame(originalIcons[6]);
	else if (po->m_isSwing)  po->updatePlayerSwingFrame(originalIcons[7]);
	
}

bool validateLimits() {
	for (int i = 0; i <= 7; i++) {
		if (iconIdMinRange[i] > iconIdMaxRange[i])
			return false;
	}

	return true;
}

void createAlertLabel(PlayLayer* pl) {
	auto alertLabel = CCLabelBMFont::create(
		"DIC mod was disabled due to incorrectly exposed icon range!",
		"bigFont.fnt"
	);
	alertLabel->setScale(0.4f);
	alertLabel->runAction(CCFadeOut::create(6.f));

	CCPoint winSize = CCDirector::get()->getWinSize();
	alertLabel->setPosition(
		winSize.x / 2.f, 100.f
	);

	pl->addChild(alertLabel);
}

void initMod(PlayLayer* pl) {
	if (!globalEnableMod) return;

	enableMod = true;

	for (int i = 0; i <= 7; i++) {
		// init min-max range icons
		int indexMin = i; int indexMax = i;

		if (indexMin != 0) indexMin = indexMin * 2;
		indexMax = indexMin + 1;

		iconIdMinRange[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMin]);
		iconIdMaxRange[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMax]);
	}

	if (!validateLimits()) {
		createAlertLabel(pl);
		enableMod = false;
	} else {
		log::debug("!!! Mod is enable !!!");
	}

}

void disableMod(PlayerObject* po) {
	resetOriginalIcons(po);
	enableMod = false;
	log::debug("!!! Mod is disable !!!");
}

class $modify(MyPlayLayer, PlayLayer) {

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		saveOriginalIcons();
		initMod(this);

		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void onQuit() {
		if (globalEnableMod) {
			disableMod(nullptr);
		}

        PlayLayer::onQuit();
    }

};

class $modify(MyPlayerObject, PlayerObject) {

	void toggleFlyMode(bool p0, bool p1) {
		setIcon(this, 1, 0, p0, p1, true);
		PlayerObject::toggleFlyMode(p0, p1);
	}

	void toggleRollMode(bool p0, bool p1) {
		setIcon(this, 2, 0, p0, p1, true);
		PlayerObject::toggleRollMode(p0, p1);
	}

	void toggleBirdMode(bool p0, bool p1) {
		setIcon(this, 3, 0, p0, p1, true);
		PlayerObject::toggleBirdMode(p0, p1);
	}

	void toggleDartMode(bool p0, bool p1) {
		setIcon(this, 4, 0, p0, p1, true);
		PlayerObject::toggleDartMode(p0, p1);
	}

	void toggleRobotMode(bool p0, bool p1) {
		setIcon(this, 5, 0, p0, p1, true);
		PlayerObject::toggleRobotMode(p0, p1);
	}

	void toggleSpiderMode(bool p0, bool p1) {
		setIcon(this, 6, 0, p0, p1, true);
		PlayerObject::toggleSpiderMode(p0, p1);
	}

	void toggleSwingMode(bool p0, bool p1) {
		setIcon(this, 7, 0, p0, p1, true);
		PlayerObject::toggleSwingMode(p0, p1);
	}
};

class $modify(MyPauseLayer, PauseLayer) {

	void customSetup() {
		PauseLayer::customSetup();
		
		auto enableModToggler = CCMenuItemToggler::createWithStandardSprites(
			this, menu_selector(MyPauseLayer::onEnableMod), 1.f
		);
		enableModToggler->setTag(69);
		enableModToggler->toggle(globalEnableMod);

		this->getChildByID("right-button-menu")->addChild(enableModToggler);
		this->getChildByID("right-button-menu")->updateLayout();
	}

public:
	void onEnableMod(CCObject* sender) {
		auto togger = static_cast<CCMenuItemToggler*>(sender);
		if (togger->getTag() != 69) return;

		togger->toggle(togger->isToggled());
		Mod::get()->setSettingValue("disable-mod", togger->isToggled());

		if (!togger->isToggled()) {
			globalEnableMod = true;
			initMod(
				static_cast<PlayLayer*>(CCDirector::get()->getRunningScene()->getChildByID("PlayLayer"))
			);
		} else {
			globalEnableMod = false;
			disableMod(gmPtr->m_gameLayer->m_player1);
		}
	}
};

class $modify(MyAppDelegate, AppDelegate) {

	void trySaveGame(bool p0) {
		/*
			loads original icons if game is 
			switched off (when mod ends incorrectly).
		*/

		if (enableMod) 
			disableMod(nullptr);

		AppDelegate::trySaveGame(p0);
	}
};
