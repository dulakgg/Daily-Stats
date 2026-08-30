#include <Geode/Geode.hpp>
#include <capeling.garage-stats-menu/include/StatsDisplayAPI.h>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

int dailyStars = Mod::get()->getSavedValue<int>("dailyStars");
int dailyMoons = Mod::get()->getSavedValue<int>("dailyMoons");

std::string getCurrentDate() {
	auto currentTime = time(0);
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	auto currentTimeinfo = std::localtime(&currentTime);
	#pragma clang diagnostic pop
	std::ostringstream oss;
	oss << std::put_time(currentTimeinfo, "%d-%m-%Y");
	return oss.str();
}

void checkAndResetDailyStats() {
    std::string currentDate = getCurrentDate();
    std::string lastDate = Mod::get()->getSavedValue<std::string>("lastDailyStatsReset");

    if (lastDate != currentDate) {
        dailyStars = 0;
		dailyMoons = 0;
        Mod::get()->setSavedValue<int>("dailyStars", 0);
		Mod::get()->setSavedValue<int>("dailyMoons", 0);
        Mod::get()->setSavedValue<std::string>("lastDailyStatsReset", currentDate);
    }
}

class $modify(GJGarageLayer) {
	bool init() {
		if (!GJGarageLayer::init())
			return false;
		
		if (Mod::get()->getSettingValue<bool>("show-in-garage")) {
			checkAndResetDailyStats();

			auto statMenu = this->getChildByID("capeling.garage-stats-menu/stats-menu");

			auto myStatItem = StatsDisplayAPI::getNewItem("daily-stars"_spr, CCSprite::create("star_small01_001.png"), dailyStars, 0.8f);
			auto myStatItem2 = StatsDisplayAPI::getNewItem("daily-moons"_spr, CCSprite::create("moon_small01_001.png"), dailyMoons, 0.8f);

			if (statMenu) {
				statMenu->addChild(myStatItem);
				statMenu->addChild(myStatItem2);
				statMenu->updateLayout();
			}

			return true;
		}
		else {
			return true;
		}

	}
};

class $modify(EndLevelLayer) { 
	void showLayer(bool toggle){
		EndLevelLayer::showLayer(toggle);
		checkAndResetDailyStats();
		dailyStars += m_stars;  
		dailyMoons += m_moons;  
		Mod::get()->setSavedValue<int>("dailyStars", dailyStars);
		Mod::get()->setSavedValue<int>("dailyMoons", dailyMoons);
	}		
};

class $modify(LevelBrowserLayer) {
	$override
	void onEnter() {
		LevelBrowserLayer::onEnter();

		if (Mod::get()->getSettingValue<bool>("show-in-browser")) {
			checkAndResetDailyStats();

			auto menu = this->getChildByID("page-menu");
			auto oldStat = menu->getChildByID("daily-stars");
			auto oldStatMoon = menu->getChildByID("daily-moons");

			if (auto oldStat = menu->getChildByID("daily-stars")) {
				oldStat->removeFromParent();
			}

			if (auto oldStatMoon = menu->getChildByID("daily-moons")) {
				oldStatMoon->removeFromParent();
			}

			auto myStatItemStars = StatsDisplayAPI::getNewItem("daily-stars"_spr, CCSprite::createWithSpriteFrameName("star_small01_001.png"), dailyStars, 1.2f);
			auto myStatItemMoons = StatsDisplayAPI::getNewItem("daily-moons"_spr, CCSprite::createWithSpriteFrameName("moon_small01_001.png"), dailyMoons, 1.2f);

			auto menuSize = menu->getContentSize();

			myStatItemStars->setPosition({
				menuSize.width / 4,
				menuSize.height - 39.f
			});

			myStatItemMoons->setPosition({
				menuSize.width / 4,
				menuSize.height - 58.f
			});

			myStatItemStars->setID("daily-stars");
			myStatItemMoons->setID("daily-moons");

			menu->addChild(myStatItemStars);
			menu->addChild(myStatItemMoons);
			
			if (Mod::get()->getSettingValue<bool>("enable-particles")) {
				CCParticleSystemQuad* starParticles = GameToolbox::particleFromString("30a-1a2a0a8a180a180a0a0a25a50a0a5a-8a0a0a10a5a0a0a0a1a0a1a0a0.25a0a1a0.05a0a0a0a0a1a0a1a0a1a0a0a0a0a0a0.35a0a0a0a20a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
				starParticles->setZOrder(-1);
				starParticles->setPosition({ 0, 0 });
				myStatItemStars->addChild(starParticles);

				CCParticleSystemQuad* moonParticles = GameToolbox::particleFromString("30a-1a2a0a8a180a180a0a0a25a50a0a5a-8a0a0a10a5a0a0a0a1a0a1a0a0.25a0a1a0.05a0a0a0a0a1a0a1a0a1a0a0a0a0a0a0.35a0a0a0a20a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
				moonParticles->setZOrder(-1);
				moonParticles->setPosition({ 0, 0 });
				myStatItemMoons->addChild(moonParticles);
			}

		}
	}
};
