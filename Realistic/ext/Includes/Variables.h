#pragma once
#include <nlohmann/json.hpp>
#include <Includes.h>

class Variables
{
public:
    //p_menu
    struct
    {
        
        bool p_Aimbot = false, p_colors = false, p_notify = false, open = false, p_Radar3D = false, p_Radar2D = false, p_Weapon = false, p_Vehicle = false, p_Misc = false, p_Settings = false, p_Authorized = false;
    }p_menu;

    //p_friend
    struct
    {
        Vector3 BasePOs = { 0,0,0 };
        std::vector<std::int32_t> Friend;
        std::vector<std::int32_t> Staff;
    }p_friend;

    //radar3d
    struct
    {
        bool p_enable3DRadar = false, p_snapline = false, p_Box = false, p_Skel = false, p_Health = true, p_Distance = true, p_Weapon = false;
        int p_renderDistance = 500;
        int healthbar_distance = 8;
        bool p_box3d = false;
        bool p_corner_box = false;
        bool visiblecheck = true;
        int p_Health_Bar_Type = 0;
        bool name = true;
        bool id = false;
        float healthbarsize = 2.032;
        bool p_Armorbar = true;
        bool include_self = false;
        bool detectAdmin = true;
        bool SpectateWarning = true;
        int size = 110;
        int bones = 0;
        int teamCheckComponent = 1;
        bool saveLocation = false;
    }radar3d;

    //color
    struct
    {
        
        ImColor Box = ImColor(0, 154, 255);
        ImColor Corner = ImColor(0, 154, 255);
        ImColor Box3d = ImColor(0, 154, 255);
        ImColor Skeleton = ImColor(128, 0, 0);
        ImColor NameTags = ImColor(0, 150, 255, 255);
        ImColor Weapon = ImColor(249, 224, 9, 255);
        ImColor Distance = ImColor(184, 239, 255, 255);
        ImColor InVisible = ImColor(0, 255, 0, 100);
        ImColor Id = ImColor(184, 239, 255,255);
        ImColor Armor = ImColor(0, 0, 255);
        ImColor PedESP = ImColor(221, 221, 221);
        ImColor Vehicle_Box = ImColor(140, 140, 255);
        ImColor Vehicle_Names = ImColor(34, 240, 240);
        ImColor Snaplines = ImColor(214, 214, 214);
        ImColor Vehicle_Snaplines = ImColor(140, 140, 255);
    }color;

    //misc
    struct
    {
        bool p_godmode = false, p_nocol = false, superjump = false, noclip = false, p_flyhack = false, p_fastrun = false, p_autoheal = false, p_seatbelt = false;
        int p_flyKey = 0x0;
        bool spinbot = false;
        bool test = false;
        bool TESTER = false;
        bool explo_meele = false;
        bool tityPed = false;
        bool ragDoll = false;
        bool infStamina = false;
        bool infRolls = false;
        bool infArmor = false;
        bool antiHS = false;
        bool AntiStun = false;
        bool clownVFX = false;
        bool invisible = false;
        float runspeed = 1.f;
        int int1 = 0;
        bool FPSBOOSTER = 0;
        bool autoFriendList = true;
        bool d3DInited = false;
        bool regen = false;
        int mOpenKey = VK_F3;
        bool freecam = false;
        bool NoCamShake = false;
        bool ModelChanger = false;
        float float1 = 0.3f;//0.96666666f;
        float float2 = 0.5f;
        float float3 = 1.0f;
        UINT32 ModelToChange = 0;
    }misc;

    //world
    struct
    {
        bool FreeCam = false;
        int cam = 0;
        bool ShootBullets = false;
        bool NoClips = false;
        bool ExplodeCar = false;
        bool DeleteCar = false;
        bool SinkCar = false;
        bool ToSky = false;
        bool DropCars = false;
        bool DragCars = false;
        bool PhysicGun = false;
        bool RcCar = false;
        bool objectEraser = false;
        bool objectSpawner = false;
        bool smashVehicle = false;
        bool breakDoors = false;
        bool breakWheels = false;
        bool burstTires = false;
        bool destroyEngine = false;
        bool setAlarm = false;
        bool washVehicle = false;
        bool TazerPlayer = false;
        bool DropHnArmor = false;
        bool BurnPlayer = false;
        bool TpAllToYou = false;
        bool silentKill = false;
        bool ParticleFx = false;
        bool DeletePed = false;
        bool AffectAll = false;
        bool includePed = false;
        bool isPauseMennuActive = false;
        PVector3 camCoords;
        UINT32 WeaponHash = false;
    }world;

    //radar2d
    struct
    {
        bool p_enable2DRadar = true;
        bool playerBlips = false;
    }radar2d;

    //p_aimbot
    struct
    {
        bool enable = true;
        bool nearest_bone = false;
        int nearest_Bone_dist = 15;
        bool normal_aim = true;
        bool silent_aim = false;
        bool magic_aim = false;
        bool prediction = true;
        bool randomSpot = false;
        bool draw_fov = true;
        int crosshair = 1;
        int distance = 200;
        bool view_aim_point = false;
        bool mouseAim = false;
        int hitChance = 100;
        bool triggerbot = false;
        int silent_fov = 20;
        int smoothness = 15;
        bool visibility_check = true;
        bool ignore_peds = false;
        int aimbot_type = 1;
        int aimbot_bone = 0;
        bool shoot_closest = false;
        int fov_size = 35;
        int Key = VK_RBUTTON;
        int Key2 = VK_LBUTTON;
        bool focusCamOnVehRot = false;
        uintptr_t currentClosestPedID = 0;
        Vector3 aimloc = { 0,0,0 };
    }p_aimbot;

    //p_vehicle
    struct
    {
        bool repair = false;
        bool GodMode = false;
        bool p_nocol = false, p_acc = false, p_gravity = false;
        bool NoFallOff = false;
        int p_acc_int = 100;
        bool p_vehicle_fly = false;
        bool warp = false;
        bool maxPerformance = false;
        bool maxVehMods = false;
        int p_grav_int = 9;
        bool p_rocket_boost = false;
        bool hornBoost = false;
        bool instantBreak = false;
        bool spawnvehicule = false;
        bool spawnInVehicle = true;
        bool spawnNetworked = true;
        bool spawn_maxed = false;
        bool BoostPlaneOnSpawn = false;
        //bool taskVehicleChase = false;
        bool nocollision = false;
        bool driveWander = false, driveCoord = false, driveWaypoint = false, stopDrivingTask = false;
        bool imOnVehicle = false;
        bool easyHandling = false;
        bool driftMode = false;
        UINT32 VehicleToSpawn = false;
        bool deleteVehicle = false;
        bool repairVehicle = false;
        bool FreeCamOpts = false;
        bool bringnSteal = false;
        int SelectedVehicle = NULL;
    }p_vehicle;

    //p_Weapons
    struct
    {
        bool explo_ammo = false;
        bool fire_ammo = false;
        bool freeze_ammo = false;
        bool No_Reload = false;
        bool inf_ammo = false;
        bool NoRecoil = false, NoSpread = false, NoReload = false, Rapid_Fire = false;
        int fireRate = 1;
        bool InfiniteAmmo = false;
        bool ammoSet = 0;
        int ammoCount = 250;
        bool range = 0;
        bool damageMultiply = false;
        float dmgModifier = 1.0;
    }p_Weapons;

    //p_settings
    struct
    {
        bool p_fps = false;
        bool p_weapon = false;
        float p_2DRadarZoom = 1;
        bool bypassWeaponCheck = false;
    }p_settings;

    //p_color
    struct
    {
        ImColor Red = ImColor(255, 68, 51, 255);
        ImColor Green = ImColor(127, 255, 0);
        ImColor Blue = ImColor(30, 144, 255);
        ImColor Aqua = ImColor(0, 255, 255);
        ImColor BlueViolet = ImColor(138, 43, 226);
        ImColor Cyan = ImColor(0, 255, 255);
        ImColor Lime = ImColor(173, 255, 47);

        ImColor BoxColor = ImColor(0, 154, 255);
        ImColor Skeleton = ImColor(255, 0, 0);
        ImColor Snaplines = ImColor(140, 140, 255, 100);
        ImColor Healthbar = ImColor(0, 255, 0);
        ImColor Armorbar = ImColor(0, 255, 255);
    }p_color;


    // ped_esp
    struct
    {
        int SelectedPedID = 0;
        const char* SelectedFx = "";
        bool enable = false;
        bool bestTarget = false;
        bool box = false; 
        bool p_Snapline = true;
        int render_distance = 1000;
        bool pedBlips = false;
        bool OnlyCarBlips = false;
    }ped_esp;
    struct
    {
        bool copyOutFit = false;
        bool shootBullet = false;
        bool destroyCar = false;
        bool giveHealth = false;
        bool giveArmor = false;
        bool rpgAndAttack = false;
        bool Chasing = false;
        bool ParticleFx = false;
        bool spam1MVehicle = false;
        bool cuffPed = false;
        bool remoteNPC = false;
        bool clonePed = false;
        bool spawnPed = false;
    }Ped_Options;
    //vehicle_esp
    struct
    {
        bool enable = false;
        bool box = false;
        bool snaplines = false;
        bool names = false;
        bool filter = true;
        int render_distance = 1000;
    }vehicle_esp;

    //object_esp
    struct
    {
        bool enable = false;
        bool lootbag = false;
        bool snaplines = false;
        bool hashes = false;
        int render_distance = 1000;
    }object_esp;


    //p_auth
    struct
    {
        bool enable = 0;
    }p_auth;

    //p_resource
    struct
    {
        bool enable = false;
        bool cache = false;
        bool excution = false;
        int targetIndex = false;
        bool replace = false;
        bool gamestate = false;
        std::string targetName = "hardcap";
        std::string svrName = "temp";
    }p_resource;

};

struct Location {
    std::string name;
    Vector3 coord;

    Location(const std::string& _name, Vector3& _coord) : name(_name), coord(_coord) {}
};

class LocationManager {
public:
    std::vector<Location> locations;
    Vector3 selected = { 0,0,0 };
    bool DrawLocEsp = false;
    void addLocation(const std::string& name, Vector3 loc) {
        locations.emplace_back(name, loc);
    }
    Vector3 findLocation(const std::string name) {
        Vector3 result;
        for (const auto& location : locations) {
            if (location.name == name)
                result = location.coord;
        }
        return result;
    }
    void removeLocation(int index) {
        if (index >= 0 && index < locations.size()) {
            locations.erase(locations.begin() + index);
        }
    }
    void saveLocations(const std::string& filename) {
        nlohmann::json jsonLocations;

        for (const auto& location : locations) {
            nlohmann::json jsonLocation;
            jsonLocation["name"] = location.name;
            jsonLocation["coord"]["x"] = location.coord.x;
            jsonLocation["coord"]["y"] = location.coord.y;
            jsonLocation["coord"]["z"] = location.coord.z;
            jsonLocations.push_back(jsonLocation);
        }

        std::ofstream file(filename);
        file << jsonLocations.dump(2); // Add indentation for better readability
        file.close();
    }
    void defaultLocations()
    {
        addLocation("Car Dealership (Simeon's)", Vector3(-3.812, -1086.427, 26.672));
        addLocation("Legion Square", Vector3(212.685, -920.016, 30.692));
        addLocation("Grove Street", Vector3(118.63, -1956.388, 20.669));
        addLocation("LSPD HQ", Vector3(436.873, -987.138, 30.69));
        addLocation("Sandy Shores BCSO HQ", Vector3(1864.287, 3690.687, 34.268));
        addLocation("Paleto Bay BCSO HQ", Vector3(-424.13, 5996.071, 31.49));
        addLocation("FIB Top Floor", Vector3(135.835, -749.131, 258.152));
        addLocation("FIB Offices", Vector3(136.008, -765.128, 242.152));
        addLocation("Michael's House", Vector3(-801.847, 175.266, 72.845));
        addLocation("Franklin's First House", Vector3(-17.813, -1440.008, 31.102));
        addLocation("Franklin's Second House", Vector3(-6.25, 522.043, 174.628));
        addLocation("Trevor's Trailer", Vector3(1972.972, 3816.498, 32.95));
        addLocation("Tequi-La-La", Vector3(-568.25, 291.261, 79.177));
    }
    void loadLocations(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open())//if save file not exist
        {//add default tp locations
            defaultLocations();
            return;
        }
        nlohmann::json jsonLocations;
        file >> jsonLocations;//parse json from file

        locations.clear();
        for (const auto& jsonLocation : jsonLocations) {
            std::string name = jsonLocation["name"];
            float x = jsonLocation["coord"]["x"];
            float y = jsonLocation["coord"]["y"];
            float z = jsonLocation["coord"]["z"];
            Vector3 coord(x, y, z);
            locations.emplace_back(name, coord);
        }
        file.close();
        if (locations.empty())
            defaultLocations();
    }

};
inline LocationManager* locationManager = new LocationManager;
inline Variables* p_variables = new Variables;