#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

// Credenciais da rede WiFi
const char* ssid = "Teste";
const char* password = "teste1234";

// Definicoes do sensor DHT
#define DHTPIN 17          // Pino conectado ao DHT11
#define DHTTYPE DHT11      // Tipo do sensor DHT

// Inicializa o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Cria o objeto server para a porta 80
AsyncWebServer server(80);

// Funcao para ler temperatura e umidade
String getSensorData(){
  // Le a umidade
  float humidity = dht.readHumidity();
  // Le a temperatura em Celsius
  float temperature = dht.readTemperature();

  // Verifica se a leitura falhou
  if (isnan(humidity) || isnan(temperature)) {
    return String("{\"temperature\":\"Erro\", \"humidity\":\"Erro\"}");
  }

  // Cria um objeto JSON com os valores
  String json = "{";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"humidity\":" + String(humidity);
  json += "}";

  return json;
}

// Codigo da pagina web que apresentara os valores:
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" 
    integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
      font-family: Arial, sans-serif;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 2.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align: middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ENergyZO</h2>
  <h3>A solução monitora a condição do ambiente, que pode influenciar no consumo de ar-condicionado, ventilação, etc.
Isso ajuda a identificar se o ambiente precisa de ajustes, como abrir janelas para ventilação natural e reduzir o uso de ar-condicionado.</h3>
  <p>
    <span class="dht-labels">Temperatura: </span> 
    <span id="temperature">--</span>&deg;C
  </p>
  <p>
    <span class="dht-labels">Umidade: </span> 
    <span id="humidity">--</span>%
  </p>
  
</body>
<script>
setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var data = JSON.parse(this.responseText);
        document.getElementById("temperature").innerHTML = data.temperature;
        document.getElementById("humidity").innerHTML = data.humidity;
      } catch (e) {
        console.error("Erro ao analisar JSON:", e);
      }
    }
  };
  xhttp.open("GET", "/sensor", true);
  xhttp.send();
}, 10000 ); // Atualiza a cada 10 segundos
</script>
</html>)rawliteral";

// Funcao de processamento de placeholders (nao utilizada nesta versao)
String processor(const String& var){
  return String();
}

void setup(){
  // Inicializa a comunicacao serial
  Serial.begin(9600);
  
  // Inicializa o sensor DHT
  dht.begin();

  // Conecta-se a rede WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando ao WiFi...");
  int maxAttempts = 20; // Tenta conectar até 20 vezes antes de desistir

  while (WiFi.status() != WL_CONNECTED && maxAttempts > 0) {
    delay(2000); // Aumenta o tempo entre tentativas
    Serial.print(".");
    maxAttempts--;
  }

  // Verifica se a conexão foi bem-sucedida ou falhou
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Conectado! Endereco IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Falha na conexão com o WiFi. Verifique as credenciais e o sinal.");
  }

  // Rota para a pagina principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Rota para fornecer os dados do sensor em formato JSON
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    String sensorData = getSensorData();
    request->send(200, "application/json", sensorData);
  });
  
  // Inicia o servidor
  server.begin();
}

void loop(){
  // Nao e necessario codigo no loop para este exemplo
}
