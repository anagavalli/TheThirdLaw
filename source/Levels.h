/*
 * Define level and level parameter here
 */

#ifndef __LEVELS_H__
#define __LEVELS_H__

#define SAVE_FILE "save.json"

// level loading constants
#define NUM_LEVELS 8

#define LEVEL_TUT1_KEY "tut1"
#define LEVEL_TUT1_FILE "json/levels/tut1.json"
#define LEVEL_TUT1_ASSETS "json/assets/tut1.json"

#define LEVEL_TUT2_KEY "tut2"
#define LEVEL_TUT2_FILE "json/levels/tut2.json"
#define LEVEL_TUT2_ASSETS "json/assets/tut2.json"

#define LEVEL_ZERO_KEY "lava_level"
#define LEVEL_ZERO_FILE "json/levels/level1.json"
#define LEVEL_ZERO_ASSETS "json/assets/level1.json"

#define LEVEL_ONE_KEY "spiral_level"
#define LEVEL_ONE_FILE "json/levels/tutorial.json"
#define LEVEL_ONE_ASSETS "json/assets/tutorial.json"

#define LEVEL_TWO_KEY "shield_boss_level"
#define LEVEL_TWO_FILE "json/levels/level3.json"
#define LEVEL_TWO_ASSETS "json/assets/level3.json"

#define LEVEL_THREE_KEY "purple_level"
#define LEVEL_THREE_FILE "json/levels/level2.json"
#define LEVEL_THREE_ASSETS "json/assets/level2.json"

#define LEVEL_FOUR_KEY "city_level"
#define LEVEL_FOUR_FILE "json/levels/level4.json"
#define LEVEL_FOUR_ASSETS "json/assets/level4.json"

#define LEVEL_FIVE_KEY "chase_boss_level"
#define LEVEL_FIVE_FILE "json/levels/level5.json"
#define LEVEL_FIVE_ASSETS "json/assets/level5.json"

typedef struct level_info {
    std::string key;
    std::string filename;
    std::string assets;
} level_info_t;

static void getLevelInfo(int levelno, level_info_t* output) {
    std::string key;
    std::string filename;
    std::string assets;
    
	if (levelno == 1) {
		key = LEVEL_TUT1_KEY;
		filename = LEVEL_TUT1_FILE;
		assets = LEVEL_TUT1_ASSETS;
	}
	else if (levelno == 2) {
		key = LEVEL_TUT2_KEY;
		filename = LEVEL_TUT2_FILE;
		assets = LEVEL_TUT2_ASSETS;
	}
	else if (levelno == 3) {
		key = LEVEL_ZERO_KEY;
		filename = LEVEL_ZERO_FILE;
		assets = LEVEL_ZERO_ASSETS;
	}
    else if (levelno == 4) {
        key = LEVEL_ONE_KEY;
        filename = LEVEL_ONE_FILE;
        assets = LEVEL_ONE_ASSETS;
    }
    else if (levelno == 5) {
        key = LEVEL_TWO_KEY;
        filename = LEVEL_TWO_FILE;
        assets = LEVEL_TWO_ASSETS;
    }
    else if (levelno == 6) {
        key = LEVEL_THREE_KEY;
        filename = LEVEL_THREE_FILE;
        assets = LEVEL_THREE_ASSETS;
    }
    else if (levelno == 7) {
        key = LEVEL_FOUR_KEY;
        filename = LEVEL_FOUR_FILE;
        assets = LEVEL_FOUR_ASSETS;
    }
    else if (levelno == 8) {
        key = LEVEL_FIVE_KEY;
        filename = LEVEL_FIVE_FILE;
        assets = LEVEL_FIVE_ASSETS;
    }
    else {
        key = LEVEL_ONE_KEY;
        filename = LEVEL_ONE_FILE;
        assets = LEVEL_ONE_ASSETS;
    }
    
    output->key = key;
    output->filename = filename;
    output->assets = assets;
}


#endif
