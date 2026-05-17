class CfgPatches
{
    class KrydenRewards
    {
        units[] = {"KrydenRewardsSeaChest"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "DZ_Gear_Camping"};
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

class CfgVehicles
{
    class SeaChest;

    class KrydenRewardsSeaChest: SeaChest
    {
        scope = 2;
        displayName = "Kryden Rewards Chest";
        descriptionShort = "Container de recompensa com insercao bloqueada para jogadores.";
        canBeDigged = 0;
        class Cargo
        {
            itemsCargoSize[] = {10,10};
            openable = 0;
            allowOwnedCargoManipulation = 1;
        };
    };
};
