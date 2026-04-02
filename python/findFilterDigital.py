from matplotlib.pyplot import subplots, show
from scipy.signal import firwin, freqz
import numpy as np

# Definindo os parâmetros do filtro
# fDBP       => Frequência em [Hz] Desejada para a Banda escolhida.
# ordem      => Ordem do Filtro.
# fs         => Frequência de Amostragem em [Hz].
# filterType => Tipo de filtro entre 'lowpass', 'highpass'
# desvio     => Máximo desvio da amplitude aceito na frequência desejada.
# isBP       => Frequência desejada é da banda de passagem "True" ou da Banda de Bloqueio "False"

def findFilterDigitalFir(fDesejada,ordem,fs,filterType="lowpass",desvio=0.05,isBP=True):
  faux      = fDesejada*(2*np.pi)/fs
  condition = (isBP if filterType=="lowpass" else not isBP)
  for fc in np.linspace(fDesejada,(10*fDesejada if fs/2 > 10*fDesejada else fs/2) if condition else fDesejada//20, 10000, endpoint=False):
    taps = firwin(ordem, cutoff = fc,fs = fs, window = "hamming", pass_zero = filterType)
    w, h = freqz(taps, 1, worN=1024)# Resposta em frequência do filtro
    hAux, wAux = (np.abs(h),np.abs(w)) if condition else (np.abs(h)[::-1], np.abs(w)[::-1])
    index = np.argmax(hAux<=(1-desvio) if isBP else hAux>=desvio)
    if ((wAux[index] >= faux) if condition else (wAux[index] <= faux)): break;

  fDInicio = wAux[index]*fs/(2*np.pi)
  index = np.argmax(hAux<=desvio if isBP else hAux>=(1-desvio))
  fDFinal = wAux[index]*fs/(2*np.pi)

  f = w*fs/(2*np.pi) #Transformando a frequência de rad/samples para Hz
  fig, ax1 = subplots(figsize=(20,6))
  ax1.set_xlabel('Frequência (Hz)')
  ax1.set_ylabel('Magnitude')
  ax1.plot(f, abs(h), 'b')
  print("Frequência limite da Banda de Passagem [Vermelho]: " + str(fDInicio if isBP else fDFinal));
  ax1.axvline(fDInicio if isBP else fDFinal,color='r');
  print("Frequência de corte [Verde]: " + str(fc));
  ax1.axvline(fc,color='g');
  print("Frequência limite da Banda de Bloqueio [Amarelo]: "+ str(fDFinal if isBP else fDInicio));
  ax1.axvline(fDFinal if isBP else fDInicio,color='y');
  ax1.grid()
  show()

  return taps

# Definindo os parâmetros do filtro
taps = findFilterDigitalFir(0.32870328703287033,50,10,isBP=True)
[print(str(h)+',') for h in taps]
#taps = findFilterDigitalFir(12,51,100,'highpass',isBP=False)
#[print(str(h)+',') for h in taps]