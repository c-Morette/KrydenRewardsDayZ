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
  "dropOnGroundIfInventoryFull": true,
  "debugLogs": true,
  "useLocalTestResponse": false,
  "localTestResponsePath": "$profile:KrydenRewards/TestRedeemResponse.json"
}
```

Nunca coloque serverKey real dentro do PBO publico. A chave real deve ficar somente no profile do servidor.

O comando de resgate e fixo no codigo do mod:

```text
//resgatar CODIGO
```

Ele nao deve ficar no `Config.json`. Isso reduz erro humano na instalacao e garante que bot, API, documentacao e mod usem sempre o mesmo comando.

## Instalacao Limpa No Servidor

Estrutura esperada depois de empacotar/publicar o mod:

```text
@KrydenRewards/
  Addons/
    KrydenRewards.pbo
    KrydenRewards.pbo.bisign
  Keys/
    KrydenRewards.bikey
```

Passo a passo:

```text
1. Copiar @KrydenRewards para a pasta raiz do servidor DayZ.
2. Copiar KrydenRewards.bikey para a pasta Keys do servidor.
3. Adicionar @KrydenRewards na linha de mods do servidor.
4. Iniciar o servidor uma primeira vez.
5. Confirmar que o arquivo $profile:KrydenRewards/Config.json foi criado.
6. Parar o servidor.
7. Editar $profile:KrydenRewards/Config.json.
8. Preencher apiBaseUrl com a URL HTTPS da instancia.
9. Preencher serverKey com a chave DayZ da instancia.
10. Ajustar dropOnGroundIfInventoryFull e debugLogs se necessario.
11. Iniciar o servidor novamente.
12. Testar //resgatar CODIGO com um pedido aprovado.
```

Exemplo final de `Config.json` em producao:

```json
{
  "apiBaseUrl": "https://cliente-a.rewards.kryden.com.br",
  "serverKey": "CHAVE_REAL_ENTREGUE_PELA_KRYDEN",
  "dropOnGroundIfInventoryFull": true,
  "debugLogs": false
}
```

## Modo Local De Teste

Para validar a geracao dos itens sem depender da API, o mod agora suporta um JSON local no profile do servidor.

Configuracao recomendada para teste:

```json
{
  "apiBaseUrl": "https://cliente-a.rewards.kryden.com.br",
  "serverKey": "COLE_A_DAYZ_SERVER_API_KEY_AQUI",
  "dropOnGroundIfInventoryFull": true,
  "debugLogs": true,
  "useLocalTestResponse": true,
  "localTestResponsePath": "$profile:KrydenRewards/TestRedeemResponse.json"
}
```

Quando `useLocalTestResponse=true`:

- o preview nao consulta a API
- a confirmacao tambem nao consulta a API
- o mod carrega o payload em `localTestResponsePath`
- se o arquivo nao existir, o mod cria um template automaticamente no primeiro uso

Arquivo esperado para teste:

```text
$profile:KrydenRewards/TestRedeemResponse.json
```

Modelo no repositorio para referencia:

```text
TestRedeemResponse.example.json
```

## Atualizacao De Config Antiga

Versoes antigas de teste podiam gerar:

```json
{
  "commandPrefix": "//resgatar"
}
```

Esse campo nao e mais usado.

Ao atualizar servidor antigo, remova `commandPrefix` manualmente do `Config.json` para evitar confusao operacional.

O comando oficial permanece fixo:

```text
//resgatar CODIGO
```

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

Referencia principal para a implementacao futura do backend:

```text
CONTRATO-JSON-API-V2.md
```

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

O formato antigo continua suportado.

Na V2, cada item tambem pode conter `attachments` e `cargo` de forma recursiva.

Exemplo resumido:

```json
{
  "items": [
    {
      "className": "KrydenRewardsSeaChest",
      "quantity": 1,
      "cargo": [
        {
          "className": "BandageDressing",
          "quantity": 4
        },
        {
          "className": "AKM",
          "quantity": 1,
          "attachments": [
            {
              "className": "Mag_AKM_30Rnd"
            },
            {
              "className": "PSO1Optic"
            }
          ]
        }
      ]
    }
  ]
}
```

Regras fixas do runtime atual:

- itens, armas, roupas, caixas e containers nascem no chao perto do jogador
- veiculos nascem a frente do jogador
- veiculos usam distancia fixa de 10 metros
- magazines anexados nascem cheios
- o mod tenta chamber automatico em armas com magazine valido
- veiculos nascem com fluidos completos
- attachments repetidos devem ser enviados como entradas repetidas no array
- `KrydenRewardsSeaChest` nao pode ser pego, nao pode ir para inventario, so permite retirada e apaga quando fica vazio

Classificacao de veiculo no runtime:

- o JSON nao envia um campo manual dizendo se algo e veiculo
- o mod decide isso pelo `className`
- atualmente, classes que herdam de `CarScript` ou `Boat_Base` entram no fluxo de veiculo

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
4. Confirmar que `commandPrefix` nao existe mais no `Config.json`.
5. Criar pedido no Discord e simular ou confirmar pagamento.
6. Digitar `//resgatar CODIGO` no DayZ.
7. Verificar se itens aparecem no inventario ou no chao.
8. Verificar se API marcou o pedido como `Claimed`.
9. Tentar resgatar o mesmo codigo novamente e confirmar bloqueio.
10. Testar codigo invalido e confirmar mensagem amigavel.
11. Testar classe invalida em pacote de teste antes de entregar a cliente.
12. Testar inventario cheio com `dropOnGroundIfInventoryFull=true`.

## Cuidados

- O comando de chat e interceptado no cliente e enviado por RPC vanilla para o servidor.
- A entrega e confirmada na API somente depois de tentar criar os itens.
- Se inventario estiver cheio e `dropOnGroundIfInventoryFull=true`, o item cai no chao perto do jogador.
- O primeiro MVP usa `GET_now`, seguindo o exemplo `VirtualLockSyncApi`.
- Depois podemos evoluir para `RestCallback` assincrono se houver travamento perceptivel.
- Nao embutir `serverKey` real no PBO.
- Nao entregar `Config.json` real dentro do pacote publico.
- Para cliente final, `debugLogs=false` e recomendado depois dos testes.



