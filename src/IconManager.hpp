#pragma once

#include "includes.hpp"

#if APPLE
    static bool isMacOs = true;
#else
    static bool isMacOs = false;
#endif

class IconManager {

    int savedIconKit[8];
    GameManager* gmPtr;
    
    inline static int iconLimits[8] = {485, 169, 118, 149, 96, 68, 69, 43};

public:
    IconManager()
        : savedIconKit{1, 1, 1, 1, 1, 1, 1, 1}, gmPtr(GameManager::get()) {};
    ~IconManager() {};

    void setIcon(
        int gamemodeId, int iconId
    );
    void updateIcon(
        PlayerObject* po, int gamemodeId, int iconId
    );
    void setAndUpdateIcon(
        PlayerObject* po, int gamemodeId, int iconId
    );

    void saveIconKit();
    void loadIconKit();
    void loadAndUpdateIconKit(
        PlayerObject* po, int activeMode
    );

    std::vector<int>* getUnlockIcons(int iconType);
    static int* getIconLimits() { return iconLimits; }

    void updateRobotAnimationSpeed(PlayerObject* po);
};