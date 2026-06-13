class KrydenRewardsConfig
{
    string apiBaseUrl = "https://{SOLICITAR_COM_A_KRYDEN}.rewards.kryden.com.br";
    string serverKey = "COLE_A_DAYZ_SERVER_API_KEY_AQUI";
    bool dropOnGroundIfInventoryFull = true;
    bool debugLogs = true;
    bool useLocalTestResponse = false;
    string localTestResponsePath = KrydenRewardsConstants.LOCAL_TEST_RESPONSE_PATH;
    string coinPlayerDatabaseDir = KrydenRewardsConstants.COIN_PLAYER_DATABASE_DIR;
    static KrydenRewardsConfig Load()
    {
        if (!FileExist(KrydenRewardsConstants.PROFILE_DIR))
        {
            MakeDirectory(KrydenRewardsConstants.PROFILE_DIR);
        }
        if (!FileExist(KrydenRewardsConstants.CONFIG_PATH))
        {
            KrydenRewardsConfig placeholder = new KrydenRewardsConfig();
            string saveError;
            JsonFileLoader<KrydenRewardsConfig>.SaveFile(KrydenRewardsConstants.CONFIG_PATH, placeholder, saveError);
            Print("[KrydenRewards] Config placeholder created at " + KrydenRewardsConstants.CONFIG_PATH + ". Fill serverKey before using rewards.");
            return placeholder;
        }
        KrydenRewardsConfig config = new KrydenRewardsConfig();
        string errorMessage;
        if (!JsonFileLoader<KrydenRewardsConfig>.LoadFile(KrydenRewardsConstants.CONFIG_PATH, config, errorMessage))
        {
            Print("[KrydenRewards] Failed to load config: " + errorMessage);
            return new KrydenRewardsConfig();
        }

        config.ApplyDefaultsIfMissing();
        return config;
    }
    bool IsConfigured()
    {
        if (useLocalTestResponse)
        {
            return localTestResponsePath != "";
        }

        return apiBaseUrl != "" && serverKey != "" && serverKey != "COLE_A_DAYZ_SERVER_API_KEY_AQUI";
    }

    string GetCoinPlayerDatabaseDir()
    {
        string normalizedPath = coinPlayerDatabaseDir;

        if (normalizedPath == "")
        {
            normalizedPath = KrydenRewardsConstants.COIN_PLAYER_DATABASE_DIR;
        }

        normalizedPath.Trim();
        while (normalizedPath.Length() > 0 && normalizedPath.Substring(normalizedPath.Length() - 1, 1) == "/")
        {
            normalizedPath = normalizedPath.Substring(0, normalizedPath.Length() - 1);
        }

        return normalizedPath;
    }

    private void ApplyDefaultsIfMissing()
    {
        bool shouldSaveConfig = false;
        string saveError;

        if (coinPlayerDatabaseDir == "")
        {
            coinPlayerDatabaseDir = KrydenRewardsConstants.COIN_PLAYER_DATABASE_DIR;
            shouldSaveConfig = true;
        }

        if (!shouldSaveConfig)
        {
            return;
        }

        JsonFileLoader<KrydenRewardsConfig>.SaveFile(KrydenRewardsConstants.CONFIG_PATH, this, saveError);
        if (saveError != "")
        {
            Print("[KrydenRewards] Failed to save updated config defaults: " + saveError);
        }
    }
}
