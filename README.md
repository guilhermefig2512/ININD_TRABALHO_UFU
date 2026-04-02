# ININD_TRABALHO_UFU
------------------

**Trabalho Final Completo e Explicado da Matéria de Instrumentação Industrial**  
Universidade Federal de Uberlândia — Semestre 2025/1

**Alunos:**
- Guilherme de Paula Figueiredo — 12211EAU013
- Gian Lucca Dinalli — 12221EBI012
---
O Trabalho Consiste em:
1° Criação de um Filtro Analógico
2° Criação de um Filtro Digital
3° Calibração 
4° Caracterização
----------------------
## Descrição do Projeto

Desenvolvimento de um **Luxímetro** baseado no microcontrolador **ESP32**, capaz de medir luminosidade em lux utilizando um sensor LDR (Light Dependent Resistor). O sistema aplica filtragem digital FIR sobre o sinal lido, converte o valor filtrado em lux por meio de uma curva de calibração, e controla a intensidade de um LED via PWM de forma inversamente proporcional à luminosidade ambiente.

---

## Estrutura do Projeto

```
ININD/
├── src/
│   └── main.cpp                  # Firmware principal do ESP32
├── include/
│   └── util/
│       └── filtroFir.h           # Implementação do filtro FIR digital
├── python/
│   ├── filtro_analogico.ipynb    # Projeto do filtro analógico (Sallen-Key / Thévenin)
│   ├── findFilterDigital.py      # Geração dos coeficientes do filtro FIR digital
│   ├── 03_gera_pontos.ipynb      # Geração de pontos para caracterização
│   ├── 03_CaracterizacaoEstatica.ipynb  # Caracterização estática do sensor
│   ├── 04_calibracao.ipynb       # Calibração (curva de ajuste)
│   ├── 05_calibracao.ipynb       # Refinamento da calibração
│   ├── 02_python_Jupyter.ipynb   # Introdução e testes Python/Jupyter
│   ├── 00_instalLibPython.bat    # Script de instalação das dependências Python
│   └── dados/
│       ├── dados.txt             # Pontos de calibração (tensão × lux)
│       └── dadosexp.txt          # Dados experimentais (ADC × lux)
├── platformio.ini                # Configuração do PlatformIO (ESP32)
└── README.md
```
## Hardware Utilizado
| Componente | Descrição |
|---|---|
| ESP32 DevKit | Microcontrolador principal |
| LDR | Sensor de luminosidade (GPIO 4 — ADC) |
| LED | Indicador de intensidade (GPIO 19 — PWM) |
---
## Etapas do Trabalho
### 1° — Criação do Filtro Analógico
**Arquivo:** `python/filtro_analogico.ipynb`
O primeiro passo foi projetar um filtro analógico passa-baixa para condicionar o sinal do sensor antes da conversão analógico-digital. O projeto envolve:
1. Análise do circuito pelo **Teorema de Thévenin** para encontrar a impedância equivalente e determinar a frequência de corte real do circuito.
2. Escolha da topologia do filtro (ex: Sallen-Key).
3. Cálculo dos componentes (resistores e capacitores) para a frequência de corte desejada.
4. Verificação da resposta em frequência no notebook.
> O objetivo do filtro analógico é eliminar ruídos de alta frequência antes mesmo de o sinal chegar ao ADC do ESP32.
---
### 2° — Criação do Filtro Digital
**Arquivos:** `python/findFilterDigital.py` e `include/util/filtroFir.h`
Com o sinal já digitalizado pelo ADC, um filtro **FIR (Finite Impulse Response)** de ordem 50 foi projetado para filtragem adicional em software.
**Como rodar:**
```bash
python python/findFilterDigital.py
```
O script `findFilterDigital.py` utiliza a biblioteca `scipy.signal.firwin` com janela de Hamming para calcular automaticamente os 50 coeficientes (taps) do filtro, dados os parâmetros:
| Parâmetro | Valor |
|---|---|
| Frequência desejada | ~0.329 Hz |
| Ordem do filtro | 50 |
| Frequência de amostragem | 10 Hz |
| Tipo | Passa-baixa |
O script exibe o gráfico da resposta em frequência e **imprime no terminal os 50 taps gerados**, um por linha. Esses valores devem ser **copiados e colados diretamente** no array `filter_taps1[]` dentro de `src/main.cpp`:
```cpp
// src/main.cpp — substitua os valores abaixo pelos taps gerados pelo script
const double filter_taps1[Ordem_Filtro_Digital] = {
    0.0007197793711263183,   // <- tap 1 gerado pelo script
    0.0010193491849166425,   // <- tap 2 gerado pelo script
    // ... (50 valores no total)
    0.0007197793711263183,   // <- tap 50 gerado pelo script
};
```
> Sempre que alterar os parâmetros do filtro (frequência de corte, ordem, etc.), rode novamente o script e atualize os taps no firmware antes de recompilar.
A implementação do filtro no ESP32 está em `include/util/filtroFir.h`, usando um **buffer circular** para convolução eficiente em tempo real.
---
### 3° — Calibração
**Arquivos:** `python/04_calibracao.ipynb`, `python/05_calibracao.ipynb` e `python/dados/`
A calibração relaciona os valores lidos pelo ADC do ESP32 com os valores reais de luminosidade (em lux), medidos com um **luxímetro de referência** (utilizamos um aplicativo de luxímetro no celular).
**Procedimento passo a passo:**
**Passo 1 — Habilitar a leitura de lux no Serial Monitor**
Antes de coletar os dados, é necessário garantir que o firmware está imprimindo o valor de luminosidade no terminal serial. Em `src/main.cpp`, verifique se a linha abaixo está **descomentada**:
```cpp
Serial.print("Luminosidade: ");
Serial.println(lux);
```
Compile, grave o firmware no ESP32 e abra o **Serial Monitor** (115200 baud) no PlatformIO. O terminal exibirá os valores de lux calculados pelo ESP32 em tempo real.
**Passo 2 — Coleta dos pares de medição**
Com o Serial Monitor aberto e o aplicativo de luxímetro do celular em mãos, varie a iluminação sobre o LDR em diferentes condições (ex: ambiente escuro, luz do ambiente, luz direta do flash do celular) e colete pares de valores:
| Lux (celular — referência) | Lux (ESP32 — Serial Monitor) |
|---|---|
| ... | ... |
| ... | ... |
Repita para pelo menos 5 a 10 pontos ao longo da faixa de operação do sensor.
**Passo 3 — Ajuste da curva no Excel**
Com os pares coletados, insira os dados em uma planilha Excel:
1. Coluna A: valores do luxímetro de referência (celular)
2. Coluna B: valores lidos pelo ESP32
Em seguida:
1. Selecione os dados e insira um **Gráfico de Dispersão (XY)**
2. Clique com o botão direito na série de dados → **Adicionar Linha de Tendência**
3. Escolha o tipo **Exponencial**
4. Marque a opção **Exibir equação no gráfico**
A equação gerada pelo Excel terá o formato `y = a × e^(b×x)`. Os coeficientes `a` e `b` encontrados devem ser aplicados diretamente em `src/main.cpp`:
```cpp
// src/main.cpp — substitua os coeficientes pelos valores obtidos no Excel
lux = 1.0437 * exp(0.0019 * valorFiltrado);
//     ^ a                ^ b
```
**Passo 4 — Atualizar e recompilar o firmware**
Após substituir os coeficientes, recompile e grave o firmware. Os dados de calibração ficam registrados em `python/dados/dadosexp.txt` e os notebooks `04_calibracao.ipynb` e `05_calibracao.ipynb` documentam o processo e os gráficos gerados.
A equação de calibração obtida neste projeto foi:
```
lux = 1.0437 × e^(0.0019 × valor_filtrado)
```
---
### 4° — Caracterização Estática
**Arquivos:** `python/03_CaracterizacaoEstatica.ipynb` e `python/03_gera_pontos.ipynb`
A caracterização estática avalia o comportamento do sensor em regime permanente (sem variações dinâmicas), determinando parâmetros como:
- **Fundo de escala** e **zero** do sensor
- **Linearidade** (ou não-linearidade) da resposta
- **Sensibilidade** (variação da saída por unidade de entrada)
- **Repetibilidade** das medidas
**Procedimento:**
1. Usar o notebook `03_gera_pontos.ipynb` para gerar os pontos de medição ao longo da faixa de operação do sensor.
2. Aplicar os pontos experimentalmente e registrar as respostas.
3. Analisar os resultados em `03_CaracterizacaoEstatica.ipynb`, gerando os gráficos e parâmetros da caracterização estática.
---
## Como Compilar e Gravar o Firmware
O projeto utiliza o **PlatformIO** (extensão para VSCode).
1. Instale o [VSCode](https://code.visualstudio.com/) e a extensão [PlatformIO IDE](https://platformio.org/).
2. Abra a pasta `ININD` no VSCode.
3. Conecte o ESP32 via USB (porta configurada em `platformio.ini` como `COM10` — altere se necessário).
4. Clique em **Upload** na barra do PlatformIO ou pressione `Ctrl+Alt+U`.
5. Abra o **Serial Monitor** (115200 baud) para visualizar a luminosidade em lux em tempo real.
---
## Como Instalar as Dependências Python
```bash
cd python
00_instalLibPython.bat
```
Ou manualmente:
```bash
pip install numpy scipy matplotlib jupyter
```
---
## Dependências
| Ferramenta | Versão mínima |
|---|---|
| PlatformIO | qualquer recente |
| ESP32 Arduino Framework | via PlatformIO |
| Python | 3.8+ |
| numpy | — |
| scipy | — |
| matplotlib | — |
| jupyter | — |

