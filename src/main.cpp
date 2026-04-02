// Trabalho de Instrumentação Industrial I - LuxÍmetro 
// Guilherme de Paula Figueiredo - 12211EAU013
// Gian Lucca Dinalli - 12221EBI012

// Inclusão das Bibliotecas
#include <Wire.h>
#include <Arduino.h>
#include <math.h>
#include <util\filtroFir.h>

#define ldrPin 4  // GPIO 4 como entrada ADC
#define ledPin 19  // GPIO 19 como PWM
#define Ordem_Filtro_Digital 50 // Ordem do Filtro Digital 
#define TIME_DELAY 200 // Time em milisegundos 

// Variáveis para armazenar os valores de leitura
int ldrValue = 0, pwmValue = 0; 
float lux = 0 , voltage = 0;
DigitalFilter filter1;

unsigned long previousMillis = 0;  // Armazena o tempo anterior

//Prótotipos das funções 
void setPWM(int pwmValue); 
void readLDR();

// Taps do Filtro Digital 
const double filter_taps1[Ordem_Filtro_Digital] =  {
0.0007197793711263183,
0.0010193491849166425,
0.0013352807663532251,
0.0016234575035137145,
0.0017677335529172142,
0.0015932275230597765,
0.0009047628175280368,
-0.0004548502203830104,
-0.00253774537137863,
-0.005230038119871456,
-0.008212766972275274,
-0.010958046432392158,
-0.01276809176969419,
-0.012856887530769127,
-0.010465590856723522,
-0.00499499374190665,
0.003866854911324512,
0.01604522792892508,
0.031028016622060978,
0.047877387393219485,
0.06530759802096209,
0.08182164844320926,
0.09588902732864528,
0.10613904519585751,
0.11154061445177499,
0.11154061445177499,
0.10613904519585753,
0.09588902732864528,
0.08182164844320926,
0.0653075980209621,
0.0478773873932195,
0.031028016622060985,
0.016045227928925085,
0.003866854911324512,
-0.004994993741906652,
-0.010465590856723522,
-0.012856887530769136,
-0.012768091769694194,
-0.010958046432392165,
-0.00821276697227528,
-0.005230038119871462,
-0.0025377453713786324,
-0.0004548502203830104,
0.0009047628175280373,
0.0015932275230597765,
0.001767733552917217,
0.0016234575035137145,
0.0013352807663532266,
0.0010193491849166425,
0.0007197793711263183,
};
// Função Principal 
void setup() {
  Serial.begin(115200);             // Inicializa a comunicação serial
  pinMode(ldrPin, INPUT);           // Define o pino do LDR como entrada
  ledcSetup(0, 5000, 8);            // Configura o canal PWM, frequência 5kHz, 8 bits de resolução (0 a 255)
  ledcAttachPin(ledPin, 0);         // Conecta o canal PWM ao pino do LED
  DigitalFilter_init(&filter1, Ordem_Filtro_Digital, filter_taps1);  
}

// Função Leitura
void readLDR(){
  // Lê e filtra o valor analógico
  int analogValue = analogRead(ldrPin);
  DigitalFilter_put(&filter1, analogValue);
  int valorFiltrado = DigitalFilter_get(&filter1); //em cima do valr filtrado faz o polinomio sendo x valor filtrado 

 // Mapeia o valor filtrado para PWM e converte para tensão e luminosidade
  pwmValue = map(analogValue, 0, 4095, 255, 0); // PWM ajustado
  voltage = analogValue * (3.3 / 4095.0);
  lux = 1.0437 * exp(0.0019 * valorFiltrado); // Calcula a luminosidade em lux

  //
//final == ver sua escala, joga o flash em cima pra ser o valor maximo e desliga a luz pra ser o valor minimo, dps pega 5 valores e 5 medidas
  // Exibe o valor lido e a tensão no monitor serial
  //Serial.print("Valor Digital: ");
  //Serial.println(analogValue);
  // Serial.print("\nTensão: ");
  // Serial.print(voltage);
  Serial.print("Luminosidade: "); // V\n
  Serial.println(lux);
  // Serial.println(" lux\n");   //Valor filtrado:
  // Serial.println(valorFiltrado);
  // Serial.println();

  // Retornar o valor mapeado para ser usado na função de PWM
  setPWM(pwmValue);  
}

// Função para controlar o PWM
void setPWM(int pwmValue){
  ledcWrite(0, pwmValue);  // Controlar a intensidade do LED usando o canal 0 de PWM
}
// Loop Infinito
void loop() {
  unsigned long currentMillis = millis();  // Armazena o tempo atual

  // Verifica se o tempo decorrido desde a última execução é maior que o tempo de atraso (200ms)
  if (currentMillis - previousMillis >= TIME_DELAY) {
    previousMillis = currentMillis;  // Atualiza o tempo anterior
    readLDR();  // Chama a função de leitura do LDR
  }
}
