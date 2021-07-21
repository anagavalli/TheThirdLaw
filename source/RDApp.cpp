//
//  RDApp.cpp
//  Rocket Demo
//
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "RDApp.h"
#include "RDGameScene.h"
#include "Constants.h"
#include "Levels.h"

using namespace cugl;


#pragma mark -
#pragma mark Application State

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void RocketApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
#endif
    
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Music>(MusicLoader::alloc()->getHook());
    _assets->attach<Node>(SceneLoader::alloc()->getHook());
	_assets->attach<LevelStartup>(GenericLoader<LevelStartup>::alloc()->getHook());

    _loaded = false;
    _menued = false;
    _leveled = false;
    _menu2load = false;
    _menu2level = false;
    _selectVisited = false;
    _haveLoaded = false;
    clickedLevel = 0;
    
    // Create a "loading" screen
    _menu.init(_assets);
    _loading.init(_assets);
    
    // Que up the other assets
    AudioEngine::start();
    _assets->loadDirectoryAsync("json/assets/common.json",nullptr);
    CULog("successful load assets json");
    
    level_info_t levelInfo;
    for (int i = 1; i <= NUM_LEVELS; i++) {
        getLevelInfo(i, &levelInfo);
        _assets->loadAsync<LevelStartup>(levelInfo.key, levelInfo.filename, nullptr);
        CULog("successful load level %i json", i);
    }

	std::shared_ptr<JsonReader> savefileReader = JsonReader::alloc(SAVE_FILE);
	if (savefileReader != nullptr) {
		std::shared_ptr<JsonValue> saveJson = savefileReader->readJson();
		highestLevelFinished = saveJson->get("levels_completed")->asInt();
		lastLevelPlayed = saveJson->get("last_played")->asInt();
	}
	//Application::get()->getSaveDirectory()
	else { highestLevelFinished = 0; lastLevelPlayed = 1; }
	CULog("last level played %d", lastLevelPlayed);
    _gameplay.setCurLevel(lastLevelPlayed);
    
    Application::onStartup(); // YOU MUST END with call to parent
}

void RocketApp::saveData() {
    //highestLevelFinished = std::max(highestLevelFinished, _gameplay.getCurLevel() - 1);
    //lastLevelPlayed = std::max(1, _gameplay.getCurLevel());
    std::shared_ptr<JsonWriter> writer = JsonWriter::alloc(SAVE_FILE);
    
    auto saveJson = JsonValue::alloc(JsonValue::Type::ObjectType);
    saveJson->appendValue("levels_completed", (double) highestLevelFinished);
    saveJson->appendValue("last_played", (double)lastLevelPlayed);
    
    writer->writeJson(saveJson);
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void RocketApp::onShutdown() {
    saveData();

    _menu.dispose();
    _loading.dispose();
    _gameplay.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    
    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void RocketApp::onSuspend() {
    saveData();
    
    AudioEngine::get()->pauseAll();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void RocketApp::onResume() {
    AudioEngine::get()->resumeAll();
}


#pragma mark -
#pragma mark Application Loop

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void RocketApp::update(float timestep) {
    
    bool play = _menu.isPlay();
    bool levels = _menu.isLevels();
    bool settings = _menu.isSettings();
    bool menuClicked = _levels.isMenu();

    // ------ MENU SCENE -------
//    CULog("%d %d %d",_menued, _menu.isActive(),play);
    if (!_menued && _menu.isActive()){
		CULog("menu\n");
        _menu.update(0.01f);
    } else if (!_menued){
		CULog("menu wait for press");
        if (play){ // The play button is pressed
            CULog("PLAY HAS BEEN PRESSED");
            _menu.dispose();
            _menued = true;
            level_info_t levelInfo;
//            getLevelInfo(highestLevelFinished == NUM_LEVELS ? NUM_LEVELS : highestLevelFinished + 1, &levelInfo);
            getLevelInfo(_gameplay.getCurLevel(), &levelInfo);
            loadLevelAssets(levelInfo.assets);
        } else if (levels){// The levels button is pressed
            _menu2level = true;
            _menu.dispose();
            if (_selectVisited){// If we have already loaded level select before
                _levels.loadLevelAssets(_assets);
            } else {
                _levels.init(_assets);
            }
            _menued = true;
            _selectVisited = true;
            
        } else if (settings) {          // The settings button is pressed
			CULog("settings\n");
            // TODO
        }
    }
                            // ------ LEVEL SELECT SCENE ------
    else if (_menu2level) {
        CULog("We are in the level select scene");
        if (!_leveled && _levels.isActive()){
            _levels.update(0.01f, highestLevelFinished + 1);
        } else if (!_leveled){
            if (menuClicked){           // pressed menu button (currently its the last button)
                _levels.dispose();
                _menu.loadLevelAssets(_assets);
                _menued = false;
                _menu2level = false;
            } else {                    // pressed a specific level button
                if (_levels.isTutorial()){clickedLevel = 1;}
                else if (_levels.isLevel1()){clickedLevel = 3;}
                else if (_levels.isLevel2()){clickedLevel = 4;}
                else if (_levels.isLevel3()){clickedLevel = 5;}
                else if (_levels.isLevel4()){clickedLevel = 6;}
                else if (_levels.isLevel5()){clickedLevel = 7;}
                else if (_levels.isLevel6()){clickedLevel = 8;}
                
                _levels.dispose();
				
				level_info_t levelInfo;
				getLevelInfo(clickedLevel, &levelInfo);
				loadLevelAssets(levelInfo.assets);

                _menu2level = false;
            }
        }
    }
                            // ------ LOADING SCENE --------
    else {
        if (!_loaded && _loading.isActive()) {
			CULog("loading\n");
            _loading.update(0.01f);
        }
        else if (!_loaded) {
            _loading.dispose(); // Disables the input listeners in this mode
			CULog("clicked level %d", clickedLevel);
            if (clickedLevel == 0){
                //_gameplay.init(_assets, highestLevelFinished == NUM_LEVELS ? NUM_LEVELS : highestLevelFinished + 1);
                _gameplay.init(_assets, _gameplay.getCurLevel());
                _loaded = true;
            } else {
                _gameplay.init(_assets, clickedLevel);
                _loaded = true;
            }

            CULog("finish loading game scene");
        }

        else {
			//CULog("gameplay\n");
            if (_gameplay.isComplete()) {
                clickedLevel = _gameplay.getCurLevel();
				highestLevelFinished = std::max(highestLevelFinished, clickedLevel);
                
                CULog("%d",clickedLevel);
                CULog("%d",highestLevelFinished);
				
                _gameplay.dispose();
                std::string newDirectory;
                std::string oldDirectory;
                level_info_t levelInfo;
                
                getLevelInfo(clickedLevel, &levelInfo);
                oldDirectory = levelInfo.assets;
				_assets->unloadDirectory(oldDirectory);

				if (clickedLevel < NUM_LEVELS) {
					clickedLevel++;

					lastLevelPlayed = std::max(1, clickedLevel);

					getLevelInfo(clickedLevel, &levelInfo);
					newDirectory = levelInfo.assets;

					loadLevelAssets(newDirectory);
					_haveLoaded = false;
				}
				else {
					lastLevelPlayed = NUM_LEVELS;

					_menu.loadLevelAssets(_assets);
					_menued = false;
				}
            }
            else {
				//CULog("update gameplay %f", timestep);
                _gameplay.update(timestep);
                if (_gameplay.isQuit()){
                    CULog("highestfinishedLevel: %i", highestLevelFinished);
                    level_info_t levelInfo;
                    getLevelInfo(_gameplay.getCurLevel(), &levelInfo);
                    _assets->unloadDirectory(levelInfo.assets);
                    _gameplay.dispose();
                    _menu.loadLevelAssets(_assets);
                    _menued = false;
                }
            }
            
        }
    }

}

void RocketApp::loadLevelAssets(std::string directory) {
	_assets->loadDirectoryAsync(directory.c_str(), nullptr);
	_loaded = false;
	_loading.loadLevelAssets(_assets);
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void RocketApp::draw() {
    if (!_menued) {
        _menu.render(_batch);
    } else if (_menu2level){
        _levels.render(_batch);
    } else if (!_loaded){
        _loading.render(_batch);
    } else {
        _gameplay.render(_batch);
    }
}

