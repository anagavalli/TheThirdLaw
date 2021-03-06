//
//  RDLevelScene.h
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
#ifndef __RD_LEVEL_SCENE_H__
#define __RD_LEVEL_SCENE_H__
#include <cugl/cugl.h>



/**
 * This class is a simple loading screen for asychronous asset loading.
 *
 * The screen will display a very minimal progress bar that displays the
 * status of the asset manager.  Make sure that all asychronous load requests
 * are issued BEFORE calling update for the first time, or else this screen
 * will think that asset loading is complete.
 *
 * Once asset loading is completed, it will display a play button.  Clicking
 * this button will inform the application root to switch to the gameplay mode.
 */
class LevelScene : public cugl::Scene {
    
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // NO CONTROLLER (ALL IN SEPARATE THREAD)
    
    //    // VIEW
    //    /** The "play" button */
    //    std::shared_ptr<cugl::Button> _playButton;
    //    /** The "levels" button */
    //    std::shared_ptr<cugl::Button> _levelsButton;

    bool _menuPressed;
    bool _tutorialPressed;
    bool _level1Pressed;
    bool _level2Pressed;
    bool _level3Pressed;
    bool _level4Pressed;
    bool _level5Pressed;
    bool _level6Pressed;
    
    // MODEL
    /** The progress displayed on the screen */
    float _progress;
    /** Whether or not the player has pressed play to continue */
    bool  _completed;
    
public:
    
//    // This is for the buttons shown in the menu
//    std::shared_ptr<cugl::PolygonNode> _2menuBut;
//    std::shared_ptr<cugl::PolygonNode> _tutorialBut;
//    std::shared_ptr<cugl::PolygonNode> _level1But;
//    std::shared_ptr<cugl::PolygonNode> _level2But;
//    std::shared_ptr<cugl::PolygonNode> _level3But;
//    std::shared_ptr<cugl::PolygonNode> _level4But;
//    std::shared_ptr<cugl::PolygonNode> _level5But;
//    std::shared_ptr<cugl::PolygonNode> _level6But;
//    
//    std::shared_ptr<cugl::PolygonNode> _selectMenu;


    
    // VIEW
    /** The "menu" button */
    std::shared_ptr<cugl::Button> _2menuButton;
    /** The "levels" button */
    std::shared_ptr<cugl::Button> _tutorialButton;
    std::shared_ptr<cugl::Button> _level1Button;
    std::shared_ptr<cugl::Button> _level2Button;
    std::shared_ptr<cugl::Button> _level3Button;
    std::shared_ptr<cugl::Button> _level4Button;
    std::shared_ptr<cugl::Button> _level5Button;
    std::shared_ptr<cugl::Button> _level6Button;
    
    bool isMenu() const { return _menuPressed; }
    bool isLevel1() const { return _level1Pressed; }
    bool isLevel2() const { return _level2Pressed; }
    bool isLevel3() const { return _level3Pressed; }
    bool isLevel4() const { return _level4Pressed; }
    bool isLevel5() const { return _level5Pressed; }
    bool isLevel6() const { return _level6Pressed; }
    bool isTutorial() const { return _tutorialPressed; }
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new loading mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    LevelScene() : Scene(), _progress(0.0f) {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~LevelScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);
    
    bool loadLevelAssets(const std::shared_ptr<cugl::AssetManager>& assets);
    
    
#pragma mark -
#pragma mark Progress Monitoring
    /**
     * The method called to update the game mode.
     *
     * This method updates the progress bar amount.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep, int highestLevel);
    
    /**
     * Returns true if loading is complete, but the player has not pressed play
     *
     * @return true if loading is complete, but the player has not pressed play
     */
    bool isPending( ) const;
};

#endif /* __RD_LOADING_SCENE_H__ */

