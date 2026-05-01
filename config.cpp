class CfgPatches
{
    class KrydenRewards
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};
class CfgMods
{
    class KrydenRewards
    {
        dir = "KrydenRewards";
        name = "KrydenRewards";
        author = "Kryden";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"KrydenRewards/Scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"KrydenRewards/Scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"KrydenRewards/Scripts/5_Mission"};
            };
        };
    };
};
