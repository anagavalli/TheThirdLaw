//
//  RDMenuScene.cpp
//  Rocket Demo
//
//  This module provides a very barebones loading screen.  Most of the time you
//  will not need a loading screen, because the assets will load so fast.  But
//  just in case, this is a simple example you can use in your games.
//
//  We know from 3152 that you all like to customize this screen.  Therefore,
//  we have kept it as simple as possible so that it is easy to modify.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "RDMenuScene.h"


using namespace cugl;

/** The ID for the button listener */
#define LISTENER_ID 2,3,4

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the controller contents, making it ready for loading
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    _playPressed = false;
    _levelsPressed = false;
    _settingsPressed = false;
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/menu.json");
    auto layer = assets->get<Node>("menu");
    layer->setContentSize(dimen);
    layer->setScale(1.0f);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    _playButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homePlay"));
    _levelsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homeLevels"));
    _settingsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homeSettings"));
    
    _playButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _playPressed = true;
    });
    _levelsButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _levelsPressed = true;
    });
    _settingsButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _settingsPressed = true;
    });
    
    _playButton->activate(2);
    _levelsButton->activate(3);
    _settingsButton->activate(4);
    
    Application::get()->setClearColor(Color4(2,2,16,255));
    addChild(layer);
    return true;
}

bool MenuScene::loadLevelAssets(const std::shared_ptr<cugl::AssetManager>& assets) {
    this->_active = true;
    _assets = assets;
    
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene::init(dimen)) {
        return false;
    }
    
    _playButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homePlay"));
    _levelsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homeLevels"));
    _settingsButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("menu_background_homeSettings"));
    
    _playButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _playPressed = true;
    });
    _levelsButton->setListener([=](const std::string& name, bool down) {
        CULog("Levels button has been pressed.");
        this->_active = down;
        _levelsPressed = true;
    });
    _settingsButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _settingsPressed = true;
    });
    
    Application::get()->setClearColor(Color4(6, 6, 16, 255));
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    // Deactivate the button (platform dependent)

        _playButton->deactivate();
        _levelsButton->deactivate();
        _settingsButton->deactivate();

    /*_playButton = nullptr;
    _levelsButton = nullptr;
    _settingsButton = nullptr;*/
    _assets = nullptr;
    
    _playPressed = false;
    _levelsPressed = false;
    _settingsPressed = false;
}

#pragma mark -
#pragma mark Progress Monitoring
/**
 * The method called to update the game mode.
 *
 * This method updates the progress bar amount.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void MenuScene::update(float progress) {
    _playButton->activate(2);
    _levelsButton->activate(3);
    _settingsButton->activate(4);
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool MenuScene::isPending( ) const {
    return _playButton != nullptr && _playButton->isVisible() && _levelsButton != nullptr && _levelsButton->isVisible() && _settingsButton != nullptr && _settingsButton->isVisible();
}
