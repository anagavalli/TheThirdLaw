//
//  RDLevelScene.cpp
//  HelloWorld
//
//  Created by Michael Chapa on 5/4/18.
//  Copyright © 2018 Game Design Initiative at Cornell. All rights reserved.
//

#include "RDLevelScene.h"

using namespace cugl;

/** The ID for the button listener */
#define LISTENER_ID 5

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
bool LevelScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH/dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    } else if (!Scene::init(dimen)) {
        return false;
    }
    
    _menuPressed = false;
    _level1Pressed = false;
    _level2Pressed = false;
    _level3Pressed = false;
    _level4Pressed = false;
    _level5Pressed = false;
    _level6Pressed = false;
    _tutorialPressed = false;
    
    // IMMEDIATELY load the splash screen assets
    _assets = assets;
    _assets->loadDirectory("json/select.json");
    auto layer = assets->get<Node>("select");
    layer->setContentSize(dimen);
    layer->setScale(1);
    layer->doLayout(); // This rearranges the children to fit the screen
    
    //    _menuButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_home"));
    _level1Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level1"));
    _level2Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level2"));
    _level3Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level3"));
    _level4Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level4"));
    _level5Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level5"));
    _level6Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level6"));
    _2menuButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_home"));
    _tutorialButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_tutorial"));
    

//    // --------------- Adding Game Over Buttons -----------------
//
//    int height = dimen.getIHeight();
//    int width = dimen.getIWidth();
//
//    // Background
//    _selectMenu = std::make_shared<PolygonNode>();
//    _selectMenu->initWithTexture(_assets->get<Texture>("pause_bg"));
//
//    layer->addChild(std::dynamic_pointer_cast<Node, PolygonNode>(_selectMenu), 100);
//
//    _selectMenu->setVisible(false);
//
//    // Menu button
//    float menuscale = 0.6f;
//    _2menuBut = std::make_shared<PolygonNode>();
//    _2menuBut->initWithTexture(_assets->get<Texture>("menu"));
//
//    _2menuButton = std::make_shared<cugl::Button>();
//    _2menuButton->init(_2menuBut);
//    _2menuButton->setScale(Vec2(menuscale, menuscale));
//
//    _selectMenu->addChild(_2menuButton);
//    _2menuButton->setPosition(width*0.6, height*0.2);
//
//    // RETRY BUTTON
//    _resetBut = std::make_shared<PolygonNode>();
//    _resetBut->initWithTexture(_assets->get<Texture>("reset"));
//
//    _resetButton = std::make_shared<cugl::Button>();
//    _resetButton->init(_resetBut);
//    _resetButton->setScale(Vec2(menuscale, menuscale));
//
//    _selectMenu->addChild(_resetButton);
//
//    _resetButton->setPosition(width*0.15, height*0.2);
//
//    _resetButton->setListener([=](const std::string& name, bool down) {
//        _QuitToMenu = true;
//    });
//
    
    _level1Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level1Pressed = true;
    });
    _level2Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level2Pressed = true;
    });
    _level3Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level3Pressed = true;
    });
    _level4Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level4Pressed = true;
    });
    _level5Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level5Pressed = true;
    });
    _level6Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level6Pressed = true;
    });
    _2menuButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _menuPressed = true;
    });
    _tutorialButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _tutorialPressed = true;
    });


    //Application::get()->setClearColor(Color4(192,192,192,255));
    addChild(layer);
    return true;
}

bool LevelScene::loadLevelAssets(const std::shared_ptr<cugl::AssetManager>& assets) {
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
    
    //    _menuButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_home"));
    _level1Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level1"));
    _level2Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level2"));
    _level3Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level3"));
    _level4Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level4"));
    _level5Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level5"));
    _level6Button = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_level6"));
    _2menuButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_home"));
    _tutorialButton = std::dynamic_pointer_cast<Button>(assets->get<Node>("select_back_tutorial"));
    
    //    _menuButton->setListener([=](const std::string& name, bool down) {
    //        this->_active = down;
    //        _menuPressed = true;
    //    });
    _level1Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level1Pressed = true;
    });
    _level2Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level2Pressed = true;
    });
    _level3Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level3Pressed = true;
    });
    _level4Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level4Pressed = true;
    });
    _level5Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level5Pressed = true;
    });
    _level6Button->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _level6Pressed = true;
    });
    _2menuButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _menuPressed = true;
    });
    _tutorialButton->setListener([=](const std::string& name, bool down) {
        this->_active = down;
        _tutorialPressed = true;
    });

    //Application::get()->setClearColor(Color4(192, 192, 192, 255));
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void LevelScene::dispose() {
    // Deactivate the button (platform dependent)
    if (isPending()) {
        _2menuButton->deactivate();
        _level1Button->deactivate();
        _level2Button->deactivate();
        _level3Button->deactivate();
        _level4Button->deactivate();
        _level5Button->deactivate();
        _level6Button->deactivate();
        _tutorialButton->deactivate();
    }
    
    _2menuButton = nullptr;
    _level1Button = nullptr;
    _level2Button = nullptr;
    _level3Button = nullptr;
    _level4Button = nullptr;
    _level5Button = nullptr;
    _level6Button = nullptr;
    _tutorialButton = nullptr;

    _assets = nullptr;
    
    _menuPressed = false;
    _level1Pressed = false;
    _level2Pressed = false;
    _level3Pressed = false;
    _level4Pressed = false;
    _level5Pressed = false;
    _level6Pressed = false;
    _tutorialPressed = false;
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
void LevelScene::update(float progress, int highestLevel) {
    //_menuButton->activate(5);
    
    for(int i = 1; i <= highestLevel; i++){
        if (i == 3){
            _level1Button->activate(6);
            _level1Button->setVisible(true);
        } else if (i == 4){
            _level2Button->activate(7);
            _level2Button->setVisible(true);
        } else if (i == 5){
            _level3Button->activate(8);
            _level3Button->setVisible(true);
        } else if (i == 6){
            _level4Button->activate(9);
            _level4Button->setVisible(true);
        } else if (i == 7){
            _level5Button->activate(10);
            _level5Button->setVisible(true);
        } else if (i == 8){
            _level6Button->activate(11);
            _level6Button->setVisible(true);
        }
    
    }
    
//    _level1Button->activate(6);
//    _level2Button->activate(7);
//    _level3Button->activate(8);
//    _level4Button->activate(9);
//    _level5Button->activate(10);
//    _level6Button->activate(11);
    _2menuButton->activate(12);
    _tutorialButton->activate(23);
}

/**
 * Returns true if loading is complete, but the player has not pressed play
 *
 * @return true if loading is complete, but the player has not pressed play
 */
bool LevelScene::isPending( ) const {
    return _2menuButton != nullptr && _2menuButton->isVisible() && _level1Button != nullptr && _level1Button->isVisible() && _level2Button != nullptr && _level2Button->isVisible();
}
