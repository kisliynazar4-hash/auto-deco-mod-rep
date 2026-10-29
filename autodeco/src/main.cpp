#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <algorithm>
#include <random>
#include <sstream>
#include <vector>

using namespace geode::prelude;

static std::mt19937& rng() {
	static std::mt19937 gen{std::random_device{}()};
	return gen;
}

static float randf(float a, float b) {
	return std::uniform_real_distribution<float>(a, b)(rng());
}

static int randi(int a, int b) {
	return std::uniform_int_distribution<int>(a, b)(rng());
}

static std::vector<int> parseIDs(std::string const& s) {
	std::vector<int> out;
	std::stringstream ss(s);
	std::string tok;
	while (std::getline(ss, tok, ',')) {
		auto n = numFromString<int>(tok);
		if (n.isOk() && n.unwrap() > 0) out.push_back(n.unwrap());
	}
	return out;
}

class $modify(AutoDecoPauseLayer, EditorPauseLayer) {
	bool init(LevelEditorLayer* lel) {
		if (!EditorPauseLayer::init(lel)) return false;

		auto menu = CCMenu::create();
		menu->setID("auto-deco-menu"_spr);
		menu->setPosition({70.f, 25.f});

		auto btn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Auto Deco"),
			this,
			menu_selector(AutoDecoPauseLayer::onAutoDeco)
		);
		btn->setID("auto-deco-button"_spr);
		menu->addChild(btn);
		this->addChild(menu);
		return true;
	}

	void onAutoDeco(CCObject*) {
		auto lel = m_editorLayer;
		if (!lel || !lel->m_editorUI) return;

		auto selected = lel->m_editorUI->getSelectedObjects();
		if (!selected || selected->count() == 0) {
			FLAlertLayer::create(
				"Auto Deco",
				"Select some objects first, then open the pause menu and press <cy>Auto Deco</c>.",
				"OK"
			)->show();
			return;
		}

		auto mod = Mod::get();
		int density = static_cast<int>(mod->getSettingValue<int64_t>("density"));
		auto ids = parseIDs(mod->getSettingValue<std::string>("ids"));
		float minS = static_cast<float>(mod->getSettingValue<double>("min-scale"));
		float maxS = static_cast<float>(mod->getSettingValue<double>("max-scale"));
		bool rot = mod->getSettingValue<bool>("random-rotation");
		bool noTouch = mod->getSettingValue<bool>("no-touch");

		if (minS > maxS) std::swap(minS, maxS);
		if (ids.empty()) {
			FLAlertLayer::create("Auto Deco", "No valid object IDs in settings.", "OK")->show();
			return;
		}

		// Snapshot positions first so new objects aren't processed again.
		std::vector<CCPoint> origins;
		for (auto obj : CCArrayExt<GameObject*>(selected)) {
			if (obj) origins.push_back(obj->getPosition());
		}

		int placed = 0;
		for (auto const& p : origins) {
			for (int i = 0; i < density; i++) {
				float s = randf(minS, maxS);
				float half = 15.f; // half a grid unit
				CCPoint pos = p;

				switch (randi(0, 3)) {
					case 0: pos += ccp(randf(-half, half), half + 15.f * s); break;  // top
					case 1: pos += ccp(randf(-half, half), -half - 15.f * s); break; // bottom
					case 2: pos += ccp(-half - 15.f * s, randf(-half, half)); break; // left
					default: pos += ccp(half + 15.f * s, randf(-half, half)); break; // right
				}

				int id = ids[randi(0, static_cast<int>(ids.size()) - 1)];
				auto obj = lel->createObject(id, pos, true);
				if (!obj) continue;

				obj->updateCustomScaleX(s);
				obj->updateCustomScaleY(s);
				if (rot) obj->setRotation(static_cast<float>(randi(0, 23) * 15));
				if (noTouch) obj->m_isNoTouch = true;
				placed++;
			}
		}

		Notification::create(
			fmt::format("Auto Deco placed {} objects", placed),
			NotificationIcon::Success
		)->show();
	}
};
