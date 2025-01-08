#pragma once

#include "includes.hpp"

class IconManager {

    int savedIconKit[8];
    GameManager* gmPtr;

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

};