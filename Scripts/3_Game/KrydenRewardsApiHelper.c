class KrydenRewardsApiHelper
{
    static bool RedeemPreview(string code, string steamId, KrydenRewardsConfig config, out KrydenRewardsRedeemResponse response, out string message)
    {
        return Redeem(code, steamId, config, "", response, message);
    }
    static bool RedeemConfirm(string code, string steamId, KrydenRewardsConfig config, out KrydenRewardsRedeemResponse response, out string message)
    {
        return Redeem(code, steamId, config, "confirm", response, message);
    }
    static bool RedeemFail(string code, string steamId, KrydenRewardsConfig config, out KrydenRewardsRedeemResponse response, out string message)
    {
        return Redeem(code, steamId, config, "fail", response, message);
    }
    private static bool Redeem(string code, string steamId, KrydenRewardsConfig config, string action, out KrydenRewardsRedeemResponse response, out string message)
    {
        response = null;
        message = "";
        if (!config || !config.IsConfigured())
        {
            message = "Config do Kryden Rewards incompleta. Preencha Config.json no profile do servidor.";
            return false;
        }

        if (config.useLocalTestResponse)
        {
            return LoadLocalTestResponse(code, steamId, config, action, response, message);
        }

        string baseUrl = config.apiBaseUrl;
        baseUrl.Trim();
        while (baseUrl.Length() > 0 && baseUrl.Substring(baseUrl.Length() - 1, 1) == "/")
        {
            baseUrl = baseUrl.Substring(0, baseUrl.Length() - 1);
        }
        string url = baseUrl + "/api/dayz/redeem/" + code + "?steamId=" + steamId + "&serverKey=" + config.serverKey;
        if (action != "")
        {
            url = url + "&action=" + action;
        }
        string raw = DoGet(url);
        if (raw == "")
        {
            message = "API nao respondeu.";
            return false;
        }

        string knownError = GetKnownErrorMessage(raw);
        if (knownError != "")
        {
            message = knownError;
            return false;
        }

        KrydenRewardsErrorResponse errorResponse = new KrydenRewardsErrorResponse();
        string errorParseMessage;
        if (raw.Contains("\"error\"") && JsonFileLoader<KrydenRewardsErrorResponse>.LoadData(raw, errorResponse, errorParseMessage))
        {
            message = ToPlayerFriendlyError(errorResponse.error, errorResponse.message);
            return false;
        }

        response = new KrydenRewardsRedeemResponse();
        string parseMessage;
        if (!JsonFileLoader<KrydenRewardsRedeemResponse>.LoadData(raw, response, parseMessage))
        {
            Print("[KrydenRewards] Invalid API response: " + parseMessage + " raw=" + raw);
            message = "Nao foi possivel validar este resgate agora. Tente novamente em instantes.";
            return false;
        }
        return true;
    }

    private static bool LoadLocalTestResponse(string code, string steamId, KrydenRewardsConfig config, string action, out KrydenRewardsRedeemResponse response, out string message)
    {
        string responsePath = config.localTestResponsePath;
        string parseMessage;

        response = null;
        message = "";

        if (responsePath == "")
        {
            responsePath = KrydenRewardsConstants.LOCAL_TEST_RESPONSE_PATH;
        }

        EnsureLocalTestResponseExists(responsePath);

        response = new KrydenRewardsRedeemResponse();
        if (!JsonFileLoader<KrydenRewardsRedeemResponse>.LoadFile(responsePath, response, parseMessage))
        {
            Print("[KrydenRewards] Failed to load local test response: " + parseMessage + " path=" + responsePath);
            message = "Falha ao ler o JSON local de teste.";
            return false;
        }

        if (!response.items)
        {
            response.items = new array<ref KrydenRewardsRedeemItem>();
        }

        if (response.requestId == "")
        {
            response.requestId = "LOCAL-TEST";
        }

        if (response.orderId == "")
        {
            response.orderId = "LOCAL-TEST";
        }

        response.redeemCode = code;
        response.steamId = steamId;

        if (action == "confirm")
        {
            response.status = "Claimed";
        }
        else if (response.status == "")
        {
            response.status = "ClaimInProgress";
        }

        return true;
    }

    private static void EnsureLocalTestResponseExists(string responsePath)
    {
        KrydenRewardsRedeemResponse sampleResponse;
        string saveError;

        if (responsePath == "" || FileExist(responsePath))
        {
            return;
        }

        sampleResponse = BuildLocalTestResponseTemplate();
        JsonFileLoader<KrydenRewardsRedeemResponse>.SaveFile(responsePath, sampleResponse, saveError);
        if (saveError != "")
        {
            Print("[KrydenRewards] Failed to create local test response template: " + saveError);
        }
    }

    private static KrydenRewardsRedeemResponse BuildLocalTestResponseTemplate()
    {
        KrydenRewardsRedeemResponse response = new KrydenRewardsRedeemResponse();
        KrydenRewardsRedeemItem seaChest = CreateRewardItem("KrydenRewardsSeaChest", 1);
        KrydenRewardsRedeemItem akm = CreateRewardItem("AKM", 1);
        KrydenRewardsRedeemItem vehicle = CreateRewardItem("OffroadHatchback", 1);

        response.orderId = "KR-LOCAL-TEST-001";
        response.requestId = "KRLOCAL0001";
        response.redeemCode = "LOCALTEST";
        response.steamId = "76561198000000000";
        response.discordUserId = "0";
        response.status = "ClaimInProgress";

        seaChest.cargo.Insert(CreateRewardItem("BandageDressing", 4));
        seaChest.cargo.Insert(CreateRewardItem("TacticalBaconCan", 2));
        akm.attachments.Insert(CreateRewardItem("Mag_AKM_30Rnd", 1));
        akm.attachments.Insert(CreateRewardItem("PSO1Optic", 1));
        akm.attachments.Insert(CreateRewardItem("AK_Suppressor", 1));
        seaChest.cargo.Insert(akm);

        vehicle.attachments.Insert(CreateRewardItem("HatchbackWheel", 1));
        vehicle.attachments.Insert(CreateRewardItem("HatchbackWheel", 1));
        vehicle.attachments.Insert(CreateRewardItem("HatchbackWheel", 1));
        vehicle.attachments.Insert(CreateRewardItem("HatchbackWheel", 1));
        vehicle.attachments.Insert(CreateRewardItem("CarBattery", 1));
        vehicle.attachments.Insert(CreateRewardItem("SparkPlug", 1));
        vehicle.attachments.Insert(CreateRewardItem("CarRadiator", 1));

        response.items.Insert(seaChest);
        response.items.Insert(CreateRewardItem("PlateCarrierVest", 1));
        response.items.Get(1).attachments.Insert(CreateRewardItem("PlateCarrierPouches", 1));
        response.items.Get(1).attachments.Insert(CreateRewardItem("PlateCarrierHolster", 1));
        response.items.Insert(vehicle);
        return response;
    }

    private static KrydenRewardsRedeemItem CreateRewardItem(string className, int quantity)
    {
        KrydenRewardsRedeemItem item = new KrydenRewardsRedeemItem();
        item.className = className;
        item.quantity = quantity;
        return item;
    }

    private static string GetKnownErrorMessage(string raw)
    {
        string normalized = raw;
        normalized.ToLower();

        if (normalized.Contains("alreadyclaimed") || normalized.Contains("ja foi resgatado"))
        {
            return "Este codigo ja foi resgatado.";
        }

        if (normalized.Contains("invalidcode"))
        {
            return "Codigo de resgate invalido.";
        }

        if (normalized.Contains("steamidmismatch"))
        {
            return "Este codigo pertence a outro SteamID.";
        }

        if (normalized.Contains("notreadytoclaim"))
        {
            return "Este pedido ainda nao esta disponivel para resgate.";
        }

        if (normalized.Contains("unauthorized") || normalized.Contains("401"))
        {
            return "Servidor nao autorizado. Avise a administracao.";
        }

        if (normalized.Contains("invalidsteamid"))
        {
            return "SteamID invalido para este resgate.";
        }

        if (normalized.Contains("409") || normalized.Contains("conflict"))
        {
            return "Este pedido nao esta disponivel ou ja foi resgatado.";
        }

        if (normalized == "client error" || normalized.Contains("client error"))
        {
            return "Este codigo nao esta disponivel ou ja foi resgatado.";
        }

        if (normalized.Contains("404") || normalized.Contains("not found"))
        {
            return "Codigo de resgate invalido.";
        }

        return "";
    }

    private static string ToPlayerFriendlyError(string error, string apiMessage)
    {
        if (error != "")
        {
            string known = GetKnownErrorMessage(error);
            if (known != "")
            {
                return known;
            }
        }

        if (apiMessage != "")
        {
            return apiMessage;
        }

        if (error != "")
        {
            return error;
        }

        return "Nao foi possivel concluir o resgate.";
    }

    static string DoGet(string url)
    {
        RestApi api = CreateRestApi();
        if (!api)
        {
            return "";
        }
        string host;
        string path;
        SplitUrl(url, host, path);
        if (host == "")
        {
            return "";
        }
        RestContext ctx = api.GetRestContext(host);
        if (!ctx)
        {
            return "";
        }
        return ctx.GET_now(path);
    }
    static void SplitUrl(string fullUrl, out string host, out string path)
    {
        host = "";
        path = "/";
        string protocol = "";
        string remainder = "";
        if (fullUrl.Contains("https://"))
        {
            protocol = "https://";
            remainder = fullUrl.Substring(8, fullUrl.Length() - 8);
        }
        else if (fullUrl.Contains("http://"))
        {
            protocol = "http://";
            remainder = fullUrl.Substring(7, fullUrl.Length() - 7);
        }
        else
        {
            return;
        }
        if (remainder.Contains("/"))
        {
            TStringArray parts = new TStringArray();
            remainder.Split("/", parts);
            if (parts.Count() > 0)
            {
                host = protocol + parts.Get(0);
                path = remainder.Substring(parts.Get(0).Length(), remainder.Length() - parts.Get(0).Length());
                if (path == "")
                {
                    path = "/";
                }
            }
        }
        else
        {
            host = protocol + remainder;
            path = "/";
        }
    }
}
