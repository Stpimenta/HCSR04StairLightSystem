# Documentação Básica - `Funcionamento`

Este sistema é composto por duas **tasks** sincronizadas usando **mutex**:

1. **Task de Sensores**: Responsável pela leitura e gerenciamento dos dados dos sensores de distância.
2. **Task de Fita**: Gerencia a faixa de LEDs (WS2812), manipulando as cores e efeitos conforme as leituras dos sensores.

### Objetos e Manipulação

- **Objeto Color**: Utilizado para manejar as cores dos LEDs, permitindo uma fácil modificação e aplicação de diferentes padrões de iluminação.

### Estrutura de Dados

A base do funcionamento do sistema é composta por **ponteiros**, que permitem a manipulação eficiente dos objetos por todo o programa. Essa abordagem facilita o acesso e a modificação de dados, promovendo uma gestão mais dinâmica e flexível dos recursos.

### Problemas de Conectividade

A fita de LEDs WS2812 apresenta problemas de funcionamento quando o módulo Wi-Fi está ativado. Para mitigar isso, foi implementada a opção de **desligar o Wi-Fi** usando um botão físico, garantindo um funcionamento estável do sistema.

### Persistência de Dados

As configurações são salvas utilizando **NVS** (Non-Volatile Storage), garantindo que as configurações do sistema sejam mantidas mesmo em caso de desligamento ou reset. Isso assegura que o sistema retorne ao seu estado anterior após uma interrupção de energia.





# Documentação Básica - `service_http_server_sensor_distance`

Esta documentação descreve os métodos HTTP disponíveis na classe `service_http_server_sensor_distance`, responsável por iniciar um servidor HTTP para gerenciar sensores de distância através de requisições **GET** e **PUT**.

## Estrutura de Dados `reqData`

A estrutura `reqData` armazena os dados dos sensores e LEDs passados para o servidor:

- `sensor`: Array com os dados dos sensores de distância (`sensor_distance_data_t`).
- `numSensor`: Número de sensores conectados.
- `color`: Estrutura que define as cores dos LEDs (`struct_color_t`).

## Inicialização do Servidor Web

O método `start_webserver()` é responsável por iniciar o servidor e registrar os seguintes endpoints:

- `/getDistance`: pega as distançias sensores.
- `/sensor`: Retorna as informações dos sensores.
- `/update_sensor`: Atualiza as distâncias dos sensores.
- `/update_rstrip`: Atualiza a faixa de LEDs associada aos sensores.
- `/update_CStrip`: Atualiza as cores dos LEDs RGB.
- `/getPage`: pagina html de configuração.
- `/getSaveConfig`: salva a config atual na memoria flash.
- `/getLoadConfig`: sobe a config anterior

---

## Métodos HTTP e Como Usá-los com `cURL`

### 1. **GET `/getDistance`** - Server-Sent Events (SSE)

Este endpoint fornece as distancias.

#### Exemplo de uso:

```bash
curl -N http://<ESP32_IP>/getDistance

resposta esperada

{
	"sensors":	[{
			"sensor":	0,
			"distance":	177
		}]
}

```



### 2. **GET `/sensor`** - Obtenção dos Sensores

Retorna uma lista em formato JSON com as informações dos sensores, incluindo configurações e estados.

#### Exemplo de uso:

```bash
curl http://<ESP32_IP>/sensor


Resposta esperada (JSON):
{
    "sensors": [
        {
            "sensor": 0,
            "actionDistanceMin1": 10,
            "actionDistanceMax1": 100,
            "actionDistanceMin2": 20,
            "actionDistanceMax2": 120,
            "startLed1": 5,
            "endLed1": 15,
            "startLed2": 10,
            "endLed2": 20,
            "gpioTrig": 23,
            "gpioEcho": 22
        }
    ]
}
```

### 3. **PUT `/update_sensor`** - Atualizar Distância dos Sensores

Este endpoint permite atualizar as distâncias mínimas e máximas de um sensor específico.

#### Parâmetros de Query:

- `sensor`: Número do sensor a ser atualizado.
- `stage`: Etapa de calibração.
- `min_distance`: Distância mínima.
- `max_distance`: Distância máxima.

#### Exemplo de uso:

```bash
curl -X PUT "http://<ESP32_IP>/update_sensor?sensor=0&stage=1&min_distance=15&max_distance=95"
```


### 4. **PUT `/update_rstrip`** - Atualizar Faixa de LEDs

Atualiza o intervalo de LEDs associados a um sensor específico.

#### Parâmetros de Query:

- `sensor`: Número do sensor.
- `stage`: Etapa de calibração.
- `start_led`: LED inicial da faixa.
- `end_led`: LED final da faixa.

#### Exemplo de uso:

```bash
curl -X PUT "http://<ESP32_IP>/update_rstrip?sensor=1&stage=2&start_led=5&end_led=10"
```

### 5. **PUT `/update_color_strip`** - Atualizar Cores do Strip de LEDs

Permite ajustar a cor dos LEDs RGB conectados ao sensor.

#### Parâmetros de Query:

- `red`: Intensidade da cor vermelha (0-255).
- `green`: Intensidade da cor verde (0-255).
- `blue`: Intensidade da cor azul (0-255).

#### Exemplo de uso:
```bash
curl -X PUT "http://<ESP32_IP>/update_CStrip?red=255&green=100&blue=150"
```

## 6. GET `/getPage` - Página HTML de Configuração

Retorna a página HTML para configuração do sistema.

### Exemplo de uso:

```bash
curl -X GET "http://<ESP32_IP>/getPage"
```

## 7. GET `/getSaveConfig` - Salvar Configuração Atual

Salva a configuração atual na memória flash do ESP32.

### Exemplo de uso:
```bash
curl -X GET "http://<ESP32_IP>/getSaveConfig"
```

## 8. GET `/getLoadConfig` - Carregar Configuração Anterior

Carrega a configuração anterior da memória flash do ESP32.

### Exemplo de uso:
```bash
curl -X GET "http://<ESP32_IP>/getLoadConfig"
```


