#pragma once

#include "includes.hpp"

struct PlayerStatus {

    int* iconKit;
    bool isMiniMode;

    PlayerObject* pObjOwner;

    PlayerStatus()                            : iconKit(new int[8]{1, 1, 1, 1, 1, 1, 1, 1}), isMiniMode(false), pObjOwner(nullptr) {};
    PlayerStatus(PlayerObject* po)            : iconKit(new int[8]{1, 1, 1, 1, 1, 1, 1, 1}), isMiniMode(false), pObjOwner(po)      {};
    PlayerStatus(PlayerObject* po, int* iKit) : iconKit(iKit), isMiniMode(false), pObjOwner(po) {};

    ~PlayerStatus() { delete[] iconKit; };

    void clear() {
        delete [] iconKit;
        iconKit = new int[8];
        isMiniMode = false;
        pObjOwner = nullptr;
    }

};

class IconManager {

    int savedIconKit[8];
    GameManager* gmPtr;

    PlayerStatus ps1, ps2;
    
    inline static int iconLimits[8] = {485, 169, 118, 149, 96, 68, 69, 43};

public:
    IconManager()
        : savedIconKit{1, 1, 1, 1, 1, 1, 1, 1}, gmPtr(GameManager::get()), ps1(), ps2() {}
    ~IconManager() {}

    void setIcon(
        PlayerObject* po, int gamemodeId, int iconId, bool ignoreMiniMode
    );
    void updateIcon(
        PlayerObject* po, int gamemodeId, int iconId, bool ignoreMiniMode
    );
    void setAndUpdateIcon(
        PlayerObject* po, int gamemodeId, int iconId, bool ignoreMiniMode
    );

    void saveIconKit();
    void loadIconKit();
    void loadAndUpdateIconKit(
        PlayerObject* po, int activeMode
    );

    std::vector<int>* getUnlockIcons(int iconType);
    static int* getIconLimits() { return iconLimits; }

    bool isEnableDefMiniIcons() { return this->gmPtr->getGameVariable("0060"); }

    void initPlayerStatus() { 
        ps1.pObjOwner = gmPtr->getGameLayer()->m_player1; ps2.pObjOwner = gmPtr->getGameLayer()->m_player2;
    }
    void clearPlayerStatus();

    PlayerObject* getPlayerObject(bool isFirstPlayer) { if (isFirstPlayer) return ps1.pObjOwner; return ps2.pObjOwner; }
    PlayerStatus* getPlayerStatus(bool isFirstPlayer) { if (isFirstPlayer) return &ps1; return &ps2; }
    
    #if __APPLE__
        void updateRobotAnimFix(PlayerObject* po) {;}
    #else
        void updateRobotAnimFix(PlayerObject* po) { po->updateRobotAnimationSpeed(); }
    #endif
};