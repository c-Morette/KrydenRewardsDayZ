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
    private static bool Redeem(string code, string steamId, KrydenRewardsConfig config, string action, out KrydenRewardsRedeemResponse response, out string message)
    {
        response = null;
        message = "";
        if (!config || !config.IsConfigured())
        {
            message = "Config do Kryden Rewards incompleta. Preencha Config.json no profile do servidor.";
            return false;
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

    private static string GetKnownErrorMessage(string raw)
    {
        if (raw.Contains("AlreadyClaimed"))
        {
            return "Este codigo ja foi resgatado.";
        }

        if (raw.Contains("InvalidCode"))
        {
            return "Codigo de resgate invalido.";
        }

        if (raw.Contains("SteamIdMismatch"))
        {
            return "Este codigo pertence a outro SteamID.";
        }

        if (raw.Contains("NotReadyToClaim"))
        {
            return "Este pedido ainda nao esta disponivel para resgate.";
        }

        if (raw.Contains("Unauthorized"))
        {
            return "Servidor nao autorizado. Avise a administracao.";
        }

        if (raw.Contains("InvalidSteamId"))
        {
            return "SteamID invalido para este resgate.";
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
