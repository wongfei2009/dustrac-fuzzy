#include <plugininterface.hpp>

extern "C" std::shared_ptr<PluginInfo> pluginInfo();
extern "C" void init(Game& game, PluginInfo& info, QStringList& args);
