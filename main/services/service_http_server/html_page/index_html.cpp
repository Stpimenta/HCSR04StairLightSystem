// index.cpp
#include "index_html.h"

const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 SSE Example</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }
        .sensor-card {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 16px;
            margin: 10px;
            display: inline-block;
            text-align: left;
            width: 250px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .sensor-card h3 {
            margin-top: 0;
        }
        .controls {
            margin: 20px;
            text-align: center;
        }
        .controls select,
        .controls input,
        .controls button {
            margin: 10px 0;
            padding: 5px;
            display: block;
            width: 200px;
            max-width: 90%;
            margin-left: auto;
            margin-right: auto;
        }
        .controls .input-group {
            margin: 20px 0;
        }
        .controls .input-group div {
            margin-bottom: 10px;
        }

        
    </style>
</head>
<body>
    <h1>Sensor Data via SSE</h1>
    <div id="sensorsContainer"></div>
    <div class="controls">
        <label for="sensorSelect">Select Sensor:</label>
        <select id="sensorSelect">
            <option value=""></option>
            <!-- Options will be populated by JavaScript -->
        </select>

        <label for="stageSelect">Select Stage:</label>
        <select id="stageSelect">
            <option value="1">Stage 1</option>
            <option value="2">Stage 2</option>
        </select>

        <div class="input-group">
            <div>
                <label for="minDistance">Min Distance:</label>
                <input type="number" id="minDistance" min="0" step="1" placeholder="0">
            </div>
            <div>
                <label for="maxDistance">Max Distance:</label>
                <input type="number" id="maxDistance" min="0" step="1" placeholder="0">
            </div>
        </div>
        <button id="submitDistance" onclick="putDistance()">Enviar</button>

        <div class="input-group">
            <div>
                <label for="startStrip">Start Strip:</label>
                <input type="number" id="startStrip" min="0" step="1" placeholder="0">
            </div>
            <div>
                <label for="endStrip">End Strip:</label>
                <input type="number" id="endStrip" min="0" step="1" placeholder="0">
            </div>
        </div>
        <button id="submitStrip" onclick="putRangeStrip()">Enviar</button>

        <div class="input-group colors">
            <div>
                <label for="redValue">Red:</label>
                <input type="number" id="redValue" min="0" step="1" placeholder="0" class="color-input">
            </div>
            <div>
                <label for="blueValue">Blue:</label>
                <input type="number" id="blueValue" min="0" step="1" placeholder="0" class="color-input">
            </div>
            <div>
                <label for="greenValue">Green:</label>
                <input type="number" id="greenValue" min="0" step="1" placeholder="0" class="color-input">
            </div>
        </div>
        <button id="submitColor" onclick="putColor()">Enviar</button>

        <div class="input-group">
            <button id="submitColor" onclick="saveConfig()">Salvar</button>
            <button id="submitColor" onclick="loadConfig()">Carregar</button>
        </div>

    </div>

    <script>
        class Sensor {
            constructor(sensor, actionDistanceMax1, actionDistanceMax2, actionDistanceMin1, actionDistanceMin2,
                        endLed1, endLed2, startLed1, startLed2, gpioTrig, gpioEcho, distance) {
                this.sensor = sensor;
                this.actionDistanceMax1 = actionDistanceMax1;
                this.actionDistanceMax2 = actionDistanceMax2;
                this.actionDistanceMin1 = actionDistanceMin1;
                this.actionDistanceMin2 = actionDistanceMin2;
                this.endLed1 = endLed1;
                this.endLed2 = endLed2;
                this.startLed1 = startLed1;
                this.startLed2 = startLed2;
                this.gpioTrig = gpioTrig;
                this.gpioEcho = gpioEcho;
                this.distance = distance; // Inicializa a distância
            }
        }

        let sensors = [];

        // Obtém a URL da página atual
        let url = window.location.origin; // Exemplo: "http://192.168.1.113/getPage"

        // Divide a URL em partes
        url = url.split('/'); // Resulta em ["http:", "", "192.168.1.113", "getPage"]

        // Filtra as partes vazias
        url = url.filter(part => part !== ""); // Resulta em ["http:", "192.168.1.113", "getPage"]

        // Junta as partes de volta com '/' como separador
        // Adiciona manualmente a barra após o "http:"
        url = url[0] + '//' + url[1]; // Resultado: "http://192.168.1.113"
        console.log(url);

        const inputMindistance = document.getElementById('minDistance');
        const inputMaxdistance = document.getElementById('maxDistance');
        const inputStartStrip = document.getElementById('startStrip');
        const inputEndStrip = document.getElementById('endStrip');
        const inputRed = document.getElementById('redValue');
        const inputBlue = document.getElementById('blueValue');
        const inputGreen = document.getElementById('greenValue');

        async function getSensor() {

            try {
                let response = await fetch(url+"/sensor");
                if (!response.ok) {
                    throw new Error("Erro na requisição, código:", response.status);
                }
                
                let DataJson = await response.json();
                sensors = DataJson.sensors.map(data => new Sensor(
                    data.sensor,
                    data.actionDistanceMax1,
                    data.actionDistanceMax2,
                    data.actionDistanceMin1,
                    data.actionDistanceMin2,
                    data.endLed1,
                    data.endLed2,
                    data.startLed1,
                    data.startLed2,
                    data.gpioTrig,
                    data.gpioEcho,
                    0
                ));

                // Limpa o contêiner antes de adicionar novos cartões
                const sensorsContainer = document.getElementById('sensorsContainer');
                sensorsContainer.innerHTML = '';

                // Cria cartões para cada sensor
                sensors.forEach(sensor => {
                    createSensorCard(sensor);
                });

                // Popula o select com os sensores
                populateSensorSelect();
                
            } catch (error) {
                console.error(error);
            }
        }

        function createSensorCard(sensor) {
            const sensorsContainer = document.getElementById('sensorsContainer');
            const card = document.createElement('div');
            card.className = 'sensor-card';
            card.innerHTML = `
                <h3>Sensor: ${sensor.sensor}</h3>
                <p>Distância: <span id="distance-${sensor.sensor}">${sensor.distance}</span> cm</p>
                <p>GPIO Trig: ${sensor.gpioTrig}</p>
                <p>GPIO Echo: ${sensor.gpioEcho}</p>
            `;
            sensorsContainer.appendChild(card);
        }

        function populateSensorSelect() {
            const sensorSelect = document.getElementById('sensorSelect');
            sensors.forEach(sensor => {
                const option = document.createElement('option');
                option.value = sensor.sensor;
                option.textContent = `Sensor ${sensor.sensor}`;
                sensorSelect.add(option);
            });
        }

        const sensorSelect = document.getElementById('sensorSelect');
        // Adiciona evento de mudança ao select de sensores
        sensorSelect.addEventListener('change', function() {
            renderData();
        });
            
        // Adiciona evento de mudança ao select de estágios
        const stageSelect = document.getElementById('stageSelect');
        stageSelect.addEventListener('change', function() {
            renderData();
        });

        async function  renderData ()
        {
            const selectedValueSensor = sensorSelect.value;
            const selectedValueStage = stageSelect.value;

            if(selectedValueSensor != '')
            {
                let sensor = sensors.find(sensor => sensor.sensor == selectedValueSensor);
                
                if(sensor == null)
                {
                    throw new Error("invalidParams");
                }


                if(selectedValueStage == 1)
                {
                      
                    inputMindistance.value = sensor.actionDistanceMin1;
                    inputMaxdistance.value = sensor.actionDistanceMax1;
                        
                    inputStartStrip.value = sensor.startLed1;
                    inputEndStrip.value = sensor.endLed1;

                        // console.log(sensor.actionDistanceMax1);
                        // console.log("1");
                }

                if(selectedValueStage == 2)
                {

                    inputMindistance.value = sensor.actionDistanceMin2;
                    inputMaxdistance.value = sensor.actionDistanceMax2;
                       
                    inputStartStrip.value = sensor.startLed2;
                    inputEndStrip.value = sensor.endLed2;
                }

            }
           

        }



         //put distance 
         async function putDistance ()
         {
      
            try 
            {
               
                if(!sensorSelect.value)
                {
                    throw new Error("invalidParams");
                }

                let sensor = sensors.find(sensor => sensor.sensor == sensorSelect.value);
                
               
            
                const response = await fetch(`${url}/update_sensor?sensor=${sensorSelect.value}&stage=${stageSelect.value}&min_distance=${inputMindistance.value}&max_distance=${inputMaxdistance.value}`,{
                    method:'PUT',
                });

                if (stageSelect.value == 1) {
                    sensor.actionDistanceMin1 = inputMindistance.value;
                    sensor.actionDistanceMax1 = inputMaxdistance.value;
                }

                if (stageSelect.value == 2) {
                    sensor.actionDistanceMin2 = inputMindistance.value;
                    sensor.actionDistanceMax2 = inputMaxdistance.value;
                }
                                
                alert("att com sucesso");

            } catch (error) {
                console.log(error);
                alert("error", error);
            }
         }


        //put strip

        async function putRangeStrip ()
         {
            try 
            {
                if(!sensorSelect.value)
                {
                    throw new Error("invalidParams");
                }

                let sensor = sensors.find(sensor => sensor.sensor == sensorSelect.value);
                
                const response = await fetch(`${url}/update_rstrip?sensor=${sensorSelect.value}&stage=${stageSelect.value}&start_led=${inputStartStrip.value}&end_led=${inputEndStrip.value}`,{
                    method:'PUT',
                });

                if (stageSelect.value == 1) {
                    sensor.startLed1 = inputEndStrip.value; 
                    sensor.endLed1 = inputStartStrip.value;
                }

                if (stageSelect.value == 2) {
                    sensor.startLed2 = inputEndStrip.value; 
                    sensor.endLed2 = inputStartStrip.value;
                }
                                
                alert("att com sucesso");

            } catch (error) {
                console.log(error);
                alert("error", error);
            }
         }


         async function putColor ()
         {
            try 
            {
                
                const response = await fetch(`${url}/updateCStrip?red=${inputRed.value}&blue=${inputBlue.value}&green=${inputGreen.value}`,{
                    method:'PUT',
                });

                                
                alert("att com sucesso");

            } catch (error) {
                console.log(error);
                alert("error", error);
            }
         }

        async function saveConfig ()
         {
            try 
            {
                const response = await fetch(`${url}/getSaveConfig`);

                if(!response.ok)
                {
                    throw new Error("erro na requisição");
                }

                alert("Salvo com sucesso");

            } catch (error) {
                console.log(error);
                alert("Erro ao salvar", error);
            }
          }

         async function loadConfig ()
         {
            try 
            {
                const response = await fetch(`${url}/getLoadConfig`);

                if(!response.ok)
                {
                    throw new Error("erro na requisição");
                }

                alert("Carregado com sucesso");

            } catch (error) {
                console.log(error);
                alert("Erro ao Carregar", error);
            }
          }

         async function getDistance ()
         {
            try 
            {
                
                const response = await fetch(`${url}/getDistance`);

                if(!response.ok)
                {
                    throw new Error("erro na requisição");
                }

                const dataJson = await response.json();
                
                dataJson.sensors.forEach(element => {

                    const distanceElement = document.getElementById(`distance-${element.sensor}`);
                    distanceElement.textContent = `${element.distance}`;
        
                });

            } catch (error) {
                console.log(error);
                alert("error ao pegar distancia", error);
            }
         }





        window.onload = function() {
            getSensor();
            setInterval(getDistance, 700);
        }

    </script>
</body>
</html>

)rawliteral";
