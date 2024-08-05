#include "mod/MyMod.h"

#include <memory>

#include "ll/api/mod/RegisterHelper.h"


bool run();
bool modenable();
bool moddisable();


namespace my_mod {

static std::unique_ptr<MyMod> instance;

MyMod& MyMod::getInstance() { return *instance; }

bool MyMod::load() {
    return run();
}

bool MyMod::enable() {
    //getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    return modenable();
}

bool MyMod::disable() {
    //getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return moddisable();
}

} // namespace my_mod

LL_REGISTER_MOD(my_mod::MyMod, my_mod::instance);
