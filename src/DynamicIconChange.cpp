#include "DynamicIconChange.hpp"

IconLimits* SettingsManager::getIconLimits() {
    IconLimits* il = new IconLimits();

    for (int i = 0; i <= 7; i++) {
		// init min-max range icons
		int indexMin = i; int indexMax = i;

		if (indexMin != 0) indexMin = indexMin * 2;
		indexMax = indexMin + 1;

		il->iconMin[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMin]);
		il->iconMax[i] = Mod::get()->getSettingValue<int64_t>(nameRangeSettings[indexMax]);
	}

    return il;
}

void GUIManager::createAlertLabel(PlayLayer* pl) {
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

void GUIManager::onFlippedToggler(CCObject* sender) {
    auto togger = static_cast<CCMenuItemToggler*>(sender);
    if (togger->getTag() != 69) return;

    togger->toggle(togger->isToggled());
    Mod::get()->setSettingValue("disable-mod", togger->isToggled());

    GameManager* gmPtr = GameManager::get();
    DynamicIconChange* dic = DynamicIconChange::get();
    bool globalStatusMod = dic->getGlobalModStatus();

    if (!togger->isToggled()) {
        SettingsManager::setGlobalStatusMod(false);
        globalStatusMod = true;

        if (dic->getIconLimits() == nullptr) dic->initMod();

        dic->enableMod();
    } else {
        SettingsManager::setGlobalStatusMod(true);
        globalStatusMod = false;

        PlayerObject* player1 = gmPtr->m_playLayer->m_player1;
        int activeMode = 0;

        if (player1->m_isShip)        activeMode = 1;
        else if (player1->m_isBall)   activeMode = 2;
        else if (player1->m_isBird)   activeMode = 3;
        else if (player1->m_isDart)   activeMode = 4;
        else if (player1->m_isRobot)  activeMode = 5;
        else if (player1->m_isSpider) activeMode = 6;
        else if (player1->m_isSwing)  activeMode = 7;

        dic->disableModInGame(player1, activeMode);
    }
}

DynamicIconChange* DynamicIconChange::get() {
    static DynamicIconChange* instance = nullptr;

    if (instance == nullptr) {
        instance = new DynamicIconChange;
        return instance;
    } else {
        return instance;
    }
}

void DynamicIconChange::initInstance(DynamicIconChange* instance) {
    if (DynamicIconChange::initClass) return;

    instance->modStatus = false;
    instance->globalModStatus = !SettingsManager::getGlobalStatusMod();
    instance->im = new IconManager;
    instance->il = nullptr;

    DynamicIconChange::initClass = true;
}

void DynamicIconChange::initMod() {
    il = SettingsManager::getIconLimits();

    if (!this->validateLimits()) return;

    log::debug("INIT MOD");
}

void DynamicIconChange::enableMod() {
    if (!this->globalModStatus) return;
    im->saveIconKit();

    modStatus = true;


    log::debug("ENABLE MOD");
}

void DynamicIconChange::disableMod() {
    if (!this->globalModStatus) return;
    
    this->im->loadIconKit();

    modStatus = false;

    log::debug("DISABLE MOD");
}

void DynamicIconChange::disableModInGame(PlayerObject* po, int activeMode) {
    if (!this->globalModStatus) return;

    modStatus = false;
    this->im->loadAndUpdateIconKit(po, activeMode);

    log::debug("DISABLE MOD (IN GAME)");
}

bool DynamicIconChange::validateLimits() {
    for (int i = 0; i <= 7; i++) {
		if (il->iconMin[i] > il->iconMax[i]) {
            log::debug("VALIDATE FAIL");
            return false;
        }
	}

    log::debug("VALIDATE DONE");
	return true;
}

inline int DynamicIconChange::generateRandIcon(int gamemodeId) {
	return (this->gen() % (il->iconMax[gamemodeId] - il->iconMin[gamemodeId] + 1)) + il->iconMin[gamemodeId];
}

void DynamicIconChange::changeMode(
    PlayerObject* po, int gamemodeId, bool p0, bool p1
) {
    if (!this->modStatus) return;

    if (!p0 && !p1) {
        if (po->m_isShip || po->m_isBall || po->m_isBird || po->m_isDart || 
		    po->m_isRobot || po->m_isSpider || po->m_isSwing) return;  // if gamemode is not a cube
        
        gamemodeId = 0;
    }

    int iconId = this->generateRandIcon(gamemodeId);

    if (po == nullptr) im->setIcon(gamemodeId, iconId);
    else               im->setAndUpdateIcon(po, gamemodeId, iconId);
}
