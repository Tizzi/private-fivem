#include <Config.h>

Config_Manager::Config_Manager()
{
    if (!this->DoesCfgDirExist("C:\\RealisticT"))
    {
        _wmkdir(L"C:\\RealisticT\\");
    }
    else
    {
        if (!this->DoesCfgDirExist("C:\\RealisticT\\cfg"))
        {
            _wmkdir(L"C:\\RealisticT\\cfg\\");
        }

        std::ifstream input("C:\\RealisticT\\cfg\\default.ini");
        for (std::string line; std::getline(input, line);)
        {
            ConfigToggle cfg = SplitLine(line);

            this->SetOptions(cfg);
        }
    }
}

bool Config_Manager::IsNumber(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void Config_Manager::Load(std::string name)
{
    if (!this->DoesCfgDirExist("C:\\RealisticT"))
    {
        _wmkdir(L"C:\\RealisticT\\");
    }
    else
    {
        if (!this->DoesCfgDirExist("C:\\RealisticT\\cfg"))
        {
            _wmkdir(L"C:\\RealisticT\\cfg\\");
        }

        if (!this->DoesCfgDirExist("C:\\RealisticT\\Saves"))
        {
            _wmkdir(L"C:\\RealisticT\\Saves\\");
        }//catche

        std::ifstream input("C:\\RealisticT\\cfg\\" + name);
        for (std::string line; std::getline(input, line);)
        {
            ConfigToggle cfg = SplitLine(line);

            this->SetOptions(cfg);
        }
        input.close();
    }
}

std::vector<std::string> Config_Manager::GetConfigs()
{
    std::string folder = "C:\\RealisticT\\cfg";
    std::vector<std::string> names;
    std::string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                names.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}


void Config_Manager::Save(std::string name)
{

    if (!this->DoesCfgDirExist("C:\\RealisticT"))
    {
        _wmkdir(L"C:\\RealisticT\\");
    }
    else
    {
        if (!this->DoesCfgDirExist("C:\\RealisticT\\cfg"))
        {
            _wmkdir(L"C:\\RealisticT\\cfg\\");
        }

        std::ofstream output;
        output.open("C:\\RealisticT\\cfg\\" + name);
        Config_Manager::ESPConfig espcfg = this->GetConfigFormat();
        Config_Manager::AimbotConfig aimbotcfg = this->GetAimbotCfg();
        for (int i = 0; i < IM_ARRAYSIZE(espcfg.toggles); i++) {
            if (output.is_open()) {
                ConfigToggle esp = espcfg.toggles[i];
                if (esp.floatval == -1) {
                    std::string espval = esp.value ? "true" : "false";
                    output << esp.identifier + "->" + espval << std::endl;
                }
                else {
                    output << esp.identifier + "->" + std::to_string(esp.floatval) << std::endl;
                }
            }
        }
        for (int i = 0; i < IM_ARRAYSIZE(aimbotcfg.toggles); i++) {
            if (output.is_open()) {
                ConfigToggle aimbot = aimbotcfg.toggles[i];
                std::string aimbotval = aimbot.value ? "true" : "false";
                output << aimbot.identifier + "->" + aimbotval << std::endl;
            }
        }
        output.close();
    }
}

void Config_Manager::SetOptions(ConfigToggle cfg)
{
   
    if (cfg.identifier == "0")
        p_variables->p_aimbot.enable = cfg.value;
    if (cfg.identifier == "1")
        p_variables->p_aimbot.normal_aim = cfg.value;
    if (cfg.identifier == "2")
        p_variables->p_aimbot.silent_aim = cfg.value;
    if (cfg.identifier == "3")
        p_variables->p_aimbot.magic_aim = cfg.value;
    if (cfg.identifier == "4")
        p_variables->p_aimbot.draw_fov = cfg.value;
    //if (cfg.identifier == "5")
    //    p_variables->p_aimbot.distance = cfg.value;
    if (cfg.identifier == "6")
        p_variables->p_aimbot.view_aim_point = cfg.value;
    //if (cfg.identifier == "7")
    //    p_variables->p_aimbot.hitChance = cfg.value;
    if (cfg.identifier == "8")
        p_variables->p_aimbot.triggerbot = cfg.value;
    if (cfg.identifier == "9")
        p_variables->p_aimbot.ignore_peds = cfg.value;
    //if (cfg.identifier == "10")
    //    p_variables->p_aimbot.smoothness = cfg.value;
    if (cfg.identifier == "11")
        p_variables->p_aimbot.visibility_check = cfg.value;
    if (cfg.identifier == "12")
        p_variables->p_aimbot.aimbot_bone = cfg.value;
    //if (cfg.identifier == "13")
    //    p_variables->p_aimbot.fov_size = cfg.value;
    //if (cfg.identifier == "14")
    //    p_variables->p_aimbot.Key = cfg.value;
   
    if (cfg.identifier == "20")
        p_variables->radar3d.p_enable3DRadar = cfg.value;
    if (cfg.identifier == "21")
        p_variables->radar3d.p_Box = cfg.value;
    if (cfg.identifier == "22")
        p_variables->radar3d.p_Skel = cfg.value;
    if (cfg.identifier == "23")
        p_variables->radar3d.p_Health = cfg.value;
    if (cfg.identifier == "24")
        p_variables->radar3d.p_Distance = cfg.value;
    if (cfg.identifier == "25")
        p_variables->radar3d.p_Weapon = cfg.value;
    //if (cfg.identifier == "26")
    //    p_variables->radar3d.p_renderDistance = cfg.value;
    if (cfg.identifier == "27")
        p_variables->radar3d.p_corner_box = cfg.value;
    if (cfg.identifier == "28")
        p_variables->radar3d.visiblecheck = cfg.value;
    if (cfg.identifier == "29")
        p_variables->radar3d.name = cfg.value;
    if (cfg.identifier == "30")
        p_variables->radar3d.id = cfg.value;
    if (cfg.identifier == "31")
        p_variables->radar3d.p_Armorbar = cfg.value;
    if (cfg.identifier == "32")
        p_variables->radar3d.include_self = cfg.value;
    if (cfg.identifier == "33")
        p_variables->radar2d.p_enable2DRadar = cfg.value;
    if (cfg.identifier == "34")
        p_variables->vehicle_esp.enable = cfg.value;
    if (cfg.identifier == "35")
        p_variables->vehicle_esp.names = cfg.value;
    if (cfg.identifier == "36")
        p_variables->vehicle_esp.box = cfg.value;
    if (cfg.identifier == "37")
        p_variables->ped_esp.enable = cfg.value;
}

Config_Manager::AimbotConfig Config_Manager::GetAimbotCfg()
{
    AimbotConfig cfg;

    cfg.toggles[0].identifier = "0";
    cfg.toggles[0].value = p_variables->p_aimbot.enable;

    cfg.toggles[1].identifier = "1";
    cfg.toggles[1].value = p_variables->p_aimbot.normal_aim;

    cfg.toggles[2].identifier = "2";
    cfg.toggles[2].value = p_variables->p_aimbot.silent_aim;

    cfg.toggles[3].identifier = "3";
    cfg.toggles[3].value = p_variables->p_aimbot.magic_aim;

    cfg.toggles[4].identifier = "4";
    cfg.toggles[4].value = p_variables->p_aimbot.draw_fov;

    //cfg.toggles[5].identifier = "5";
    //cfg.toggles[5].value = p_variables->p_aimbot.distance;

    cfg.toggles[6].identifier = "6";
    cfg.toggles[6].value = p_variables->p_aimbot.view_aim_point;

    //cfg.toggles[7].identifier = "7";
    //cfg.toggles[7].value = p_variables->p_aimbot.hitChance;

    cfg.toggles[8].identifier = "8";
    cfg.toggles[8].value = p_variables->p_aimbot.triggerbot;

    cfg.toggles[9].identifier = "9";
    cfg.toggles[9].value = p_variables->p_aimbot.ignore_peds;

    //cfg.toggles[10].identifier = "10";
    //cfg.toggles[10].value = p_variables->p_aimbot.smoothness;

    cfg.toggles[11].identifier = "11";
    cfg.toggles[11].value = p_variables->p_aimbot.visibility_check;

    cfg.toggles[12].identifier = "12";
    cfg.toggles[12].value = p_variables->p_aimbot.aimbot_bone;

    //cfg.toggles[13].identifier = "13";
    //cfg.toggles[13].value = p_variables->p_aimbot.fov_size;

    //cfg.toggles[14].identifier = "14";
    //cfg.toggles[14].value = p_variables->p_aimbot.Key;
    return cfg;
}

Config_Manager::ESPConfig Config_Manager::GetConfigFormat()
{
    ESPConfig cfg;

    cfg.toggles[0].identifier = "20";
    cfg.toggles[0].value = p_variables->radar3d.p_enable3DRadar;

    cfg.toggles[1].identifier = "21";
    cfg.toggles[1].value = p_variables->radar3d.p_Box;
    
    cfg.toggles[2].identifier = "22";
    cfg.toggles[2].value = p_variables->radar3d.p_Skel;

    cfg.toggles[3].identifier = "23";
    cfg.toggles[3].value = p_variables->radar3d.p_Health;

    cfg.toggles[4].identifier = "24";
    cfg.toggles[4].value = p_variables->radar3d.p_Distance;

    cfg.toggles[5].identifier = "25";
    cfg.toggles[5].value = p_variables->radar3d.p_Weapon;

    //cfg.toggles[6].identifier = "26";
    //cfg.toggles[6].value = p_variables->radar3d.p_renderDistance;

    cfg.toggles[7].identifier = "27";
    cfg.toggles[7].value = p_variables->radar3d.p_corner_box;

    cfg.toggles[8].identifier = "28";
    cfg.toggles[8].value = p_variables->radar3d.visiblecheck;

    cfg.toggles[9].identifier = "29";
    cfg.toggles[9].value = p_variables->radar3d.name;

    cfg.toggles[10].identifier = "30";
    cfg.toggles[10].value = p_variables->radar3d.id;

    cfg.toggles[11].identifier = "31";
    cfg.toggles[11].value = p_variables->radar3d.p_Armorbar;

    cfg.toggles[12].identifier = "32";
    cfg.toggles[12].value = p_variables->radar3d.include_self;

    cfg.toggles[13].identifier = "33";
    cfg.toggles[13].value = p_variables->radar2d.p_enable2DRadar;

    cfg.toggles[14].identifier = "34";
    cfg.toggles[14].value = p_variables->vehicle_esp.enable;

    cfg.toggles[15].identifier = "35";
    cfg.toggles[15].value = p_variables->vehicle_esp.names;

    cfg.toggles[16].identifier = "36";
    cfg.toggles[16].value = p_variables->vehicle_esp.box;
    
    cfg.toggles[17].identifier = "37";
    cfg.toggles[17].value = p_variables->ped_esp.enable;

    return cfg;
}

int Config_Manager::DoesCfgDirExist(const char* path)
{
    struct stat info;
    if (stat(path, &info) != 0)
        return 0;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

bool Config_Manager::DoesFileExist(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}


Config_Manager::ConfigToggle Config_Manager::SplitLine(std::string s)
{
    ConfigToggle cfg;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '-' && s[i + 1] == '>') {
            cfg.identifier = s.substr(0, i);
            std::string val = s.substr(i + 2, s.length());
            if (this->IsNumber(val)) {
                cfg.floatval = std::stof(val);
            }
            if (val == "true") {
                cfg.value = true;
            }
            else if (val == "false") {
                cfg.value = false;
            }
        }
    }
    return cfg;
}