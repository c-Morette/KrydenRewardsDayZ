# KrydenRewards - DayZ Mod

Este mod consome a API do Kryden Rewards para permitir que o jogador resgate uma compra dentro do jogo usando:

```text
//resgatar CODIGO
```

## Fluxo

1. Jogador compra no Discord.
2. API aprova o pedido e gera `redeemCode`.
3. Bot informa `//resgatar CODIGO`.
4. Jogador digita o comando no chat do DayZ.
5. O cliente intercepta o comando e envia RPC para o servidor.
6. O servidor pega o SteamID64 real do jogador.
7. O servidor chama a API via GET.
8. A API retorna os itens.
9. O mod entrega os itens no inventario ou no chao.
10. O mod chama a API com `action=confirm`.
11. A API marca o pedido como `Claimed`.

## Config Do Servidor

O mod gera automaticamente este arquivo no profile do servidor se ele nao existir.

Isso acontece durante o `MissionServer.OnInit()`, quando o servidor inicia:

```text
$profile:KrydenRewards/Config.json
```

Exemplo:

```json
{
  "apiBaseUrl": "https://cliente-a.rewards.kryden.com.br",
  "serverKey": "COLE_A_DAYZ_SERVER_API_KEY_AQUI",
  "commandPrefix": "//resgatar",
  "dropOnGroundIfInventoryFull": true,
  "debugLogs": true
}
```

Nunca coloque serverKey real dentro do PBO publico. A chave real deve ficar somente no profile do servidor.

## Endpoint Consumido

Preview/inicio do resgate:

```text
GET /api/dayz/redeem/{code}?steamId={steamId}&serverKey={serverKey}
```

Confirmacao apos entregar os itens:

```text
GET /api/dayz/redeem/{code}?steamId={steamId}&serverKey={serverKey}&action=confirm
```

## JSON Esperado Da API

O mod preserva os nomes vindos da API:

```json
{
  "orderId": "KR-20260501170014743",
  "requestId": "KRPIX000000001",
  "redeemCode": "KR-SMCCTR",
  "steamId": "76561198181942294",
  "discordUserId": "271828552314388481",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "BandageDressing",
      "quantity": 4
    }
  ]
}
```

## Arquitetura Do Mod

```text
config.cpp
Config.example.json
Scripts/
  3_Game/
    KrydenRewardsApiHelper.c
    KrydenRewardsConfig.c
    KrydenRewardsConstants.c
    KrydenRewardsModels.c
  4_World/
    KrydenRewardsRedeemService.c
    PlayerBase.c
  5_Mission/
    ChatInputMenu.c
    MissionServer.c
```

## Pontos De Teste

1. Subir o mod em servidor local.
2. Confirmar que `$profile:KrydenRewards/Config.json` foi gerado ao iniciar o servidor.
3. Preencher `serverKey` real da instancia.
4. Criar pedido no Discord e simular pagamento.
5. Digitar `//resgatar CODIGO` no DayZ.
6. Verificar se itens aparecem no inventario ou no chao.
7. Verificar se API marcou o pedido como `Claimed`.
8. Tentar resgatar o mesmo codigo novamente e confirmar bloqueio.

## Cuidados

- O comando de chat e interceptado no cliente e enviado por RPC vanilla para o servidor.
- A entrega e confirmada na API somente depois de tentar criar os itens.
- Se inventario estiver cheio e `dropOnGroundIfInventoryFull=true`, o item cai no chao perto do jogador.
- O primeiro MVP usa `GET_now`, seguindo o exemplo `VirtualLockSyncApi`.
- Depois podemos evoluir para `RestCallback` assincrono se houver travamento perceptivel.
