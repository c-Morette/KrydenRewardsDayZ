# KrydenRewards DayZ Mod - Especificacao Tecnica V2

## 1. Objetivo

Este documento define a especificacao tecnica para a evolucao do sistema de recompensas do KrydenRewards no DayZ, com suporte a pacotes compostos.

O foco desta especificacao e permitir validacao tecnica primeiro com JSON local dentro do servidor DayZ, antes de qualquer alteracao em API, banco ou Admin App.

Este documento descreve:

- contrato tecnico do JSON V2
- regras de execucao no servidor DayZ
- regras de classificacao por tipo de entidade
- comportamento esperado de rollback
- regras de confirmacao futura com API
- matriz de testes recomendada

## 2. Principios

### 2.1 Implementacao em camadas

A V2 deve ser implementada em tres camadas, nesta ordem:

1. runtime do DayZ com JSON local
2. transporte do mesmo contrato pela API
3. configuracao operacional via Admin App

### 2.2 Compatibilidade

O formato antigo baseado em `className + quantity` deve continuar funcionando sem alteracao.

Se um item nao possuir campos V2, o mod deve usar o fluxo simples atual.

### 2.3 Confirmacao tardia

O resgate so pode ser confirmado na API depois que toda a recompensa tiver sido criada com sucesso.

### 2.4 Sem transacao nativa

O DayZ nao possui transacao real de spawn de objetos.

Portanto, a V2 deve adotar modelo de rollback de melhor esforco, com rastreamento de todas as entidades criadas durante a tentativa de resgate.

## 3. Escopo da V2

### 3.1 Suportado

- item simples
- item empilhavel com quantidade
- item nao empilhavel com multiplas unidades
- mochila ou container com `cargo`
- roupa ou colete com `attachments`
- arma com `attachments`
- magazine sempre cheio por regra do mod
- tentativa automatica de chamber por regra do mod
- veiculo com `attachments`
- veiculo sempre nasce com fluidos cheios
- spawn de veiculo a frente do jogador com distancia fixa no mod

### 3.2 Fora do escopo inicial

- pool aleatoria de loot
- spawnChance por item
- maxItems/minItems por pacote
- editor visual no Admin App nesta fase
- suporte a objetos de base especiais sem validacao previa
- transacao real atomica

## 4. Contrato tecnico do JSON V2

### 4.1 Estrutura de resposta

O JSON local de teste deve espelhar ao maximo a futura resposta da API.

Formato base:

```json
{
  "orderId": "KR-LOCAL-TEST-001",
  "requestId": "KRLOCAL0001",
  "redeemCode": "LOCALTEST",
  "steamId": "76561198000000000",
  "discordUserId": "0",
  "status": "ClaimInProgress",
  "items": []
}
```

### 4.2 Estrutura do item V2

Cada item do array `items` representa uma entidade raiz da recompensa.

Campos permitidos:

```json
{
  "className": "MountainBag_Blue",
  "quantity": 1,
  "attachments": [],
  "cargo": []
}
```

### 4.3 Semantica dos campos

`className`
- classe DayZ a ser criada
- obrigatorio

`quantity`
- quantidade logica do item
- obrigatorio
- se menor que 1, o runtime deve tratar como 1

`attachments`
- filhos criados como attachment do item raiz
- opcional
- nao possuem `quantity` editavel no contrato
- quando houver pecas repetidas, o JSON deve repetir a mesma entrada varias vezes

`cargo`
- filhos criados no inventario interno do item raiz
- opcional

### 4.4 Regras hardcoded do mod

Os campos abaixo nao devem sair no endpoint, porque representam comportamento fixo do runtime:

- `spawnMode` nao e enviado pela API
- `distanceMeters` nao e enviado pela API
- `fillFluids` nao e enviado pela API
- `ammoQuantity` nao e enviado pela API
- `chamberAmmo` nao e enviado pela API

Regras fixas iniciais:

- item simples, arma, roupa, colete, mochila e container nascem no chao perto do jogador
- veiculo nasce sempre a frente do jogador
- distancia do veiculo e constante interna do mod
- magazine em attachment nasce sempre cheio
- arma tenta chamber automaticamente ao final da montagem
- veiculo nasce sempre com fluidos completos

## 5. Regras fixas de spawn no runtime

Como o endpoint final sera fechado e opinativo, o modo de spawn nao deve ser configurado por pacote.

Regras fixas iniciais:

- item simples no chao perto do jogador
- arma no chao perto do jogador
- roupa, colete, mochila e container no chao perto do jogador
- veiculo a frente do jogador, com distancia fixa e orientacao coerente

Essas regras ficam no mod e nao no contrato da API.

## 6. Exemplo completo de JSON local V2

```json
{
  "orderId": "KR-LOCAL-TEST-001",
  "requestId": "KRLOCAL0001",
  "redeemCode": "LOCALTEST",
  "steamId": "76561198000000000",
  "discordUserId": "0",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "MountainBag_Blue",
      "quantity": 1,
      "cargo": [
        {
          "className": "BandageDressing",
          "quantity": 4
        },
        {
          "className": "TacticalBaconCan",
          "quantity": 2
        }
      ]
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
        },
        {
          "className": "AK_Suppressor"
        }
      ]
    },
    {
      "className": "PlateCarrierVest",
      "quantity": 1,
      "attachments": [
        {
          "className": "PlateCarrierPouches"
        },
        {
          "className": "PlateCarrierHolster"
        }
      ]
    },
    {
      "className": "OffroadHatchback",
      "quantity": 1,
      "attachments": [
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "CarBattery"
        },
        {
          "className": "SparkPlug"
        },
        {
          "className": "CarRadiator"
        }
      ]
    }
  ]
}
```

### 6.1 Regra para pecas repetidas

Quando o mesmo attachment precisar aparecer varias vezes, o endpoint deve repetir a entrada no array em vez de usar `quantity`.

Exemplo correto para quatro rodas:

```json
"attachments": [
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" },
  { "className": "HatchbackWheel" }
]
```

Essa decisao reduz erro operacional no editor e deixa o contrato mais consistente com attachments de slot unico.

## 7. Classificacao de entidade no runtime

Antes de executar um item V2, o runtime deve classificar a entidade raiz.

Categorias minimas:

- item simples
- container ou roupa com cargo
- roupa ou colete com attachments
- arma
- veiculo

Essa classificacao existe para escolher a estrategia correta de spawn e pos-processamento.

## 8. Modelo de execucao no DayZ

### 8.1 Fluxo geral

Fluxo recomendado para um resgate:

1. receber o blueprint completo
2. validar configuracao do mod
3. validar jogador e SteamID
4. validar estrutura basica do JSON
5. validar `className` das entidades antes do spawn
6. iniciar contexto de tentativa de resgate
7. criar cada item raiz
8. montar attachments e cargo de forma recursiva
9. aplicar pos-processamento especifico, como ammo e fluidos
10. se tudo der certo, finalizar como sucesso
11. se algo falhar, executar rollback de melhor esforco
12. somente depois disso confirmar na API

### 8.2 Contexto de tentativa

Cada resgate deve manter um contexto temporario contendo:

- `requestId`
- referencia do jogador
- lista de entidades criadas
- status de sucesso ou falha
- mensagem tecnica de falha

Toda entidade criada com sucesso deve ser registrada imediatamente nesse contexto.

## 9. Regras de criacao por categoria

### 9.1 Item simples

Regras:

- se `quantity` for 1, criar uma unidade
- se o item suportar stack, aplicar quantidade no proprio item
- se nao suportar stack, criar multiplas unidades
- criar no chao perto do jogador

### 9.2 Container com cargo

Regras:

- criar a entidade raiz
- depois criar cada item de `cargo` dentro do inventario interno da raiz
- cada filho em `cargo` tambem pode ter seus proprios `attachments` e `cargo`
- falha em qualquer filho invalida o bloco inteiro

### 9.3 Roupa ou colete com attachments

Regras:

- criar a raiz
- criar cada item de `attachments` via slots de attachment
- se existir `cargo`, preencher apos os attachments
- qualquer falha invalida o bloco inteiro

### 9.4 Arma

Regras:

- criar a arma raiz no chao perto do jogador
- criar attachments da arma
- magazines em attachment devem sair sempre cheios
- tentar alimentar a camara automaticamente ao final
- se chamber falhar, o comportamento inicial recomendado e nao abortar automaticamente o resgate, desde que a arma e o magazine estejam validos

### 9.5 Veiculo

Regras:

- nunca criar dentro do jogador
- criar sempre a frente do jogador
- usar distancia fixa definida no mod
- aplicar orientacao coerente com a direcao do jogador
- criar attachments do veiculo
- aplicar fluidos completos sempre
- se local de spawn for inseguro, falhar antes de criar

## 10. Execucao recursiva

O executor deve usar abordagem recursiva similar ao padrao comprovado por mods de loot com nesting.

Ordem recomendada para cada entidade:

1. criar a entidade atual
2. aplicar quantidade e estado basico
3. criar `attachments`
4. criar `cargo`
5. aplicar finalizacao especifica do tipo

Essa ordem minimiza inconsistencias, principalmente em armas e containers.

## 11. Regras de atributos e estado

### 11.1 Quantity

Para itens com quantidade interna:

- `quantity` em item simples representa numero logico de unidades ou stack, conforme a classe
- `quantity` em `cargo` representa quantidade logica do item interno
- `attachments` nao usam `quantity` no contrato final
- se for necessario repetir a mesma peca, a entrada deve aparecer varias vezes no array

### 11.2 Municao e chamber

Regras fixas iniciais:

- magazine criado como attachment nasce sempre com municao maxima
- chamber e tentado automaticamente na arma raiz
- chamber so deve ser tentado apos todos os attachments terem sido criados

### 11.3 Fluidos

Para veiculos:

- preencher combustivel
- preencher oleo
- preencher brake fluid
- preencher coolant

### 11.4 Distancia de spawn de veiculo

Regra fixa inicial:

- veiculo nasce sempre a frente do jogador
- distancia vem de constante interna do mod
- o contrato da API nao expoe esse valor

## 12. Validacoes recomendadas antes de spawn

Antes da criacao real, o runtime deve validar:

- `className` nao vazio
- `quantity >= 1`
- consistencia da arvore de items
- attachments sem `quantity`
- attachments repetidos apenas por repeticao explicita de entradas

Validacao minima recomendada por item:

- classe raiz valida
- filhos com classe valida
- estrutura sem loops logicos

## 13. Politica de rollback

### 13.1 Objetivo

Reduzir risco de entrega parcial.

### 13.2 Regra operacional

Se qualquer parte da recompensa falhar, o mod deve:

1. marcar a tentativa como falha
2. interromper novas criacoes
3. tentar deletar todas as entidades registradas no contexto da tentativa
4. nao confirmar a API

### 13.3 Limite conhecido

Rollback no DayZ e de melhor esforco.

Ele reduz risco, mas nao garante atomicidade total em todos os cenarios.

## 14. Politica de confirmacao futura com API

### 14.1 Preview

O preview continua sendo a fonte do blueprint.

### 14.2 Confirmacao

A confirmacao futura so deve ocorrer quando:

- todas as entidades raiz foram criadas com sucesso
- toda a arvore de attachments e cargo foi montada
- pos-processamento obrigatorio terminou
- nenhum erro invalidador ocorreu

### 14.3 Falha de confirmacao

Se os itens foram entregues, mas a confirmacao remota falhar:

- o jogador deve receber mensagem de erro operacional
- o servidor deve registrar log tecnico detalhado
- nao tentar duplicar a entrega na mesma chamada

## 15. Estrategia de teste local

### 15.1 Objetivo

Validar o executor do DayZ antes de depender da API.

### 15.2 Recomendacao

Implementar um modo local em que o servidor carregue um arquivo JSON de teste com o mesmo shape da resposta futura.

Esse modo deve:

- usar o mesmo parser das respostas remotas
- pular a confirmacao real com API
- manter o restante do fluxo identico

### 15.3 Beneficios

- prova tecnica do contrato V2
- elimina dependencia de rede na fase de teste
- evita retrabalho de schema entre DayZ e API

## 16. Matriz de testes recomendada

### 16.1 Teste A - Compatibilidade legada

Entrada:

- item simples no formato antigo

Resultado esperado:

- o fluxo atual continua funcionando

### 16.2 Teste B - Stack e nao stack

Entrada:

- item empilhavel com quantidade alta
- item nao empilhavel com quantidade alta

Resultado esperado:

- stack correta quando suportado
- multiplas unidades quando nao suportado

### 16.3 Teste C - Mochila com cargo

Entrada:

- mochila com bandagens e comida dentro

Resultado esperado:

- mochila nasce corretamente
- itens ficam dentro dela

### 16.4 Teste D - Arma montada

Entrada:

- arma
- magazine
- mira
- supressor

Resultado esperado:

- arma nasce corretamente
- attachments entram
- magazine sai sempre cheio

### 16.5 Teste E - Chamber automatico

Entrada:

- arma com magazine valido em attachment

Resultado esperado:

- validar se o chamber automatico funciona de forma confiavel por regra fixa do mod
- se nao funcionar, manter magazine cheio como fallback aceitavel

### 16.6 Teste F - Colete com attachments

Entrada:

- colete
- pouches
- holster

Resultado esperado:

- attachments entram nos slots corretos

### 16.7 Teste G - Veiculo completo

Entrada:

- veiculo com pecas e fluidos

Resultado esperado:

- spawn seguro a frente do jogador
- sem prender o jogador
- fluidos preenchidos
- pecas anexadas

### 16.8 Teste H - Falha controlada com rollback

Entrada:

- blueprint com uma classe invalida em attachment ou cargo

Resultado esperado:

- tentativa falha
- entidades ja criadas sao removidas por rollback
- API nao seria confirmada

## 17. Recomendacao de ordem de implementacao

1. parser V2 compativel com contrato antigo
2. executor recursivo para `cargo` e `attachments`
3. regra hardcoded de magazine cheio
4. tentativa automatica de chamber em arma
5. fluxo separado de veiculo
6. rollback de melhor esforco
7. modo JSON local
8. so depois integracao com API real

## 18. Decisao tecnica recomendada

A V2 deve nascer primeiro como contrato local do DayZ.

Depois que o executor composto estiver validado em jogo, a API deve apenas transportar esse mesmo blueprint, e o Admin App deve apenas editar esse mesmo contrato.

Essa abordagem reduz retrabalho, preserva compatibilidade com o MVP atual e coloca o maior risco onde ele realmente esta: no runtime do DayZ.

## 19. Endpoint final recomendado

Com as decisoes atuais, o endpoint V2 deve expor apenas os campos que realmente variam por pacote.

Formato recomendado:

```json
{
  "orderId": "KR-LOCAL-TEST-001",
  "requestId": "KRLOCAL0001",
  "redeemCode": "LOCALTEST",
  "steamId": "76561198000000000",
  "discordUserId": "0",
  "status": "ClaimInProgress",
  "items": [
    {
      "className": "MountainBag_Blue",
      "quantity": 1,
      "cargo": [
        {
          "className": "BandageDressing",
          "quantity": 4
        },
        {
          "className": "TacticalBaconCan",
          "quantity": 2
        }
      ]
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
        },
        {
          "className": "AK_Suppressor"
        }
      ]
    },
    {
      "className": "PlateCarrierVest",
      "quantity": 1,
      "attachments": [
        {
          "className": "PlateCarrierPouches"
        },
        {
          "className": "PlateCarrierHolster"
        }
      ]
    },
    {
      "className": "OffroadHatchback",
      "quantity": 1,
      "attachments": [
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "HatchbackWheel"
        },
        {
          "className": "CarBattery"
        },
        {
          "className": "SparkPlug"
        },
        {
          "className": "CarRadiator"
        }
      ]
    }
  ]
}
```

Campos removidos do endpoint por serem regra fixa do mod:

- `spawnMode`
- `distanceMeters`
- `fillFluids`
- `ammoQuantity`
- `chamberAmmo`

Regra operacional final:

- API informa somente estrutura do pacote
- mod decide comportamento fixo de spawn e inicializacao
- attachments repetidos sao representados por repeticao explicita de entradas