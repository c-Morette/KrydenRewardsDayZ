modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        KrydenRewardsConfig config = KrydenRewardsConfig.Load();
        if (!config.IsConfigured())
        {
            Print("[KrydenRewards] Config generated or incomplete. Fill $profile:KrydenRewards/Config.json before using rewards.");
            return;
        }

        if (config.debugLogs)
        {
            Print("[KrydenRewards] Config loaded. API: " + config.apiBaseUrl);
        }
    }
}
