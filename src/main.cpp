#include "includes.hpp"

#include "DynamicIconChange.hpp"

#include <Geode/loader/SettingV3.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/PlayerObject.hpp>

DynamicIconChange* dic;

void initDic() {
	dic = DynamicIconChange::get();
	DynamicIconChange::initInstance(dic);
}

$execute {
	initDic();

	listenForSettingChanges("disable-mod", [](bool value) {
		dic->getGlobalModStatus() = !value;
	});
}

class $modify(MyPlayLayer, PlayLayer) {

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		dic->initMod();
		dic->getIconManager()->saveIconKit();

		return PlayLayer::init(level, useReplay, dontCreateObjects);
	}

	void onQuit() {
		dic->disableMod();

        PlayLayer::onQuit();
    }

};

class $modify(MyPlayerObject, PlayerObject) {

	void toggleFlyMode(bool p0, bool p1) {
		dic->changeMode(this, 1, p0, p1);
		PlayerObject::toggleFlyMode(p0, p1);
	}

	void toggleRollMode(bool p0, bool p1) {
		dic->changeMode(this, 2, p0, p1);
		PlayerObject::toggleRollMode(p0, p1);
	}

	void toggleBirdMode(bool p0, bool p1) {
		dic->changeMode(this, 3, p0, p1);
		PlayerObject::toggleBirdMode(p0, p1);
	}

	void toggleDartMode(bool p0, bool p1) {
		dic->changeMode(this, 4, p0, p1);
		PlayerObject::toggleDartMode(p0, p1);
	}

	void toggleRobotMode(bool p0, bool p1) {
		dic->changeMode(this, 5, p0, p1);
		PlayerObject::toggleRobotMode(p0, p1);
	}

	void toggleSpiderMode(bool p0, bool p1) {
		dic->changeMode(this, 6, p0, p1);
		PlayerObject::toggleSpiderMode(p0, p1);
	}

	void toggleSwingMode(bool p0, bool p1) {
		dic->changeMode(this, 7, p0, p1);
		PlayerObject::toggleSwingMode(p0, p1);
	}
};

class $modify(MyPauseLayer, PauseLayer) {

	void customSetup() {  // гайс, я в плюсах зелёный, как перенести MyPauseLayer в GUIManager?
		PauseLayer::customSetup();
		
		auto enableModToggler = CCMenuItemToggler::createWithStandardSprites(
			this, menu_selector(MyPauseLayer::onFlippedToggler), 1.f
		);
		enableModToggler->setTag(69);
		enableModToggler->toggle(dic->getGlobalModStatus());

		this->getChildByID("right-button-menu")->addChild(enableModToggler);
		this->getChildByID("right-button-menu")->updateLayout();
	}

public:
	void onFlippedToggler(CCObject* sender) {
		GUIManager::onFlippedToggler(sender);
	}
};

class $modify(MyAppDelegate, AppDelegate) {

	void trySaveGame(bool p0) {
		/*
			loads original icons if game is 
			switched off (when mod ends incorrectly).
		*/

		if (dic->getModStatus()) 
			dic->disableMod();

		AppDelegate::trySaveGame(p0);
	}
};
