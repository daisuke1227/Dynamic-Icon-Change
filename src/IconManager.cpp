#include "IconManager.hpp"

void IconManager::setIcon(int gamemodeId, int iconId) {
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
    log::debug("SET ICON STATE: GM {} - {}", gamemodeId, iconId);
}

void IconManager::updateIcon(PlayerObject* po, int gamemodeId, int iconId) {
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
    log::debug("UPDATE ICON STATE: GM {} - {}", gamemodeId, iconId);
}

void IconManager::setAndUpdateIcon(PlayerObject* po, int gamemodeId, int iconId) {
	this->setIcon(gamemodeId, iconId);
	this->updateIcon(po, gamemodeId, iconId);
}

void IconManager::saveIconKit() {
    for (int i = 0; i <= 7; i++) {
		log::debug("SAVE ICON KIT");
		this->savedIconKit[i] = gmPtr->activeIconForType(static_cast<IconType>(i));
		log::debug("SAVE STATE: GM {} - {}", i, this->savedIconKit[i]);
	}
}

void IconManager::loadIconKit() {
    log::debug("LOAD ICON KIT");
	for (int i = 0; i <= 7; i++) {
		this->setIcon(i, this->savedIconKit[i]);
		log::debug("LOAD STATE: GM {} - {}", i, this->savedIconKit[i]);
	}
}

void IconManager::loadAndUpdateIconKit(PlayerObject* po, int activeMode) {
    this->loadIconKit();

    this->updateIcon(po, 0, this->savedIconKit[0]);  // First update cube
    this->updateIcon(po, activeMode, this->savedIconKit[activeMode]);
}

std::vector<int>* IconManager::getUnlockIcons(int iconType) {
	auto unlockIcons = new std::vector<int>;
	auto it = static_cast<IconType>(iconType);

	for (int i = 1; i <= this->iconLimits[iconType]; i++)
		if (this->gmPtr->isIconUnlocked(i, it)) unlockIcons->push_back(i);

	return unlockIcons;
}
