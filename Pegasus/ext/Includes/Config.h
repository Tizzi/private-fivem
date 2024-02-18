#pragma once
#include <Includes.h>

class Config_Manager {
public:
    struct ConfigToggle
    {
        std::string identifier;
        float floatval = -1;
        bool value;
    };
    struct Slider
    {
        std::string identifier;
        bool value;
    };
    struct ConfigColor {
        std::string identifier;
        ImColor color;
    };
    class ESPConfig {
    public:
        ConfigToggle toggles[18];
    };
    class AimbotConfig {
    public:
        ConfigToggle toggles[18];
    };
    Config_Manager();
    ESPConfig GetConfigFormat();
    AimbotConfig GetAimbotCfg();
    void Load(std::string name);
    std::vector<std::string> GetConfigs();
    void Save(std::string name);
    void SetOptions(ConfigToggle cfg);
private:

    int DoesCfgDirExist(const char* path);
    inline bool DoesFileExist(const std::string& name);
    bool IsNumber(std::string s);
    ConfigToggle SplitLine(std::string s);
};

inline Config_Manager* Config_Mgr = new Config_Manager; 