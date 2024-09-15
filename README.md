# TinyWD

This is a hardware watchdog, build for Arduino and other MCU directly pluggable to the ISP connector.

It's build from an Attiny 45/85. 

from here I describe the structure in German

Dies ist ein Hardware Watchdog, aufgebaut mit einem ATTIny84. Ziel ist es, eine vorhanden Controllerplatine nachträglich mit einem Watchdog auszustatten. Denn manchmal reicht der in den Chips eingebaute Watchdog nicht aus, bzw. ist nicht zuverlässig genug. So ist es mir bei meinem Tonnenpumpen Projekt ergangen. Der im ATTiny84 eingebaute Watchdog konnte den Controller nicht zuverlässig vor einem Einfrieren schützen. Immer wieder musste ich den Controller per Hand Resetten. Natürlich gibt es auch fertige Watchdogs, meist in Transistor oder DIP-8 Form. Doch die Beschaffung war schwierig und außerdem macht es doch Spaß, sowas selber zu designen. Deswegen habe ich auf Basis des ATTiny85 diesen Hardware Watchdog gebaut, der auf einen normalen 6-Pol ISP Programmierstecker aufgesteckt werden kann.  Zunächst aber habe ich eine Variante für den Digispark entworfen, um die prinzipielle Funktionsweise zu testen. Der Vorteil der ISP Aufsteckvariante ist der, das man ja üblicherweise die Pins des ISP selten für seine eigene Schaltung verwendet. Und wenn, dann doch als SPI (was direkt den WD triggern könnte) um nicht durch die eigene Schaltung das ISP zu verwirren.  																																											

# Digispark Version

![wd_digispark](./images/wd_digispark.jpg)

Die ersten Tests hab ich mit einem Digispark gemacht. Leider ergibt sich durch den USB Bootloader, dass man beim Digispark nicht alle Pins für den Watchdog verwenden kann. Bei den Pins für den USB kommt es dazu, dass der Digispark einfach nicht starten möchte. Ursache ist der Nukleus Bootloader.

# PCB Varianten

Der Digispark ist zwar nett, aber eine eigenes PCB könnte man deutlich kleiner machen. Sowohl mit als auch ohne SMD Bausteine. 

![schematic](./images/schematic.jpg)Hier der Schaltplan: Oben die normale Variante, unten die SMD. 

Hier mal die fertige Platine:

![pcb_front](./images/pcb_front.jpg)

Links ist die klassische Variante mit normalen Bauteilen, rechts die deutlich kleinere SMD Variante. (Der Chip wird auf der Unterseite montiert.)

![pcb_back](./images/pcb_back.jpg)

Hier die Rückseite. Oben im Bild sieht man auch den SOIC-8 Programmieradapter und mein Programmiergerät. 

Wichtig ist, das man vorher weiß, wo der WD eingesetzt werden soll. Will man den WD, so wie ich, direkt auf den ISP aufsetzen, ist es wichtig, die Buchse (2x3) auf der Platinen Unterseite zu platzieren. (Siehe Fotos)

Will man den WD per 6-pol Kabel mit dem ISP verbinden, braucht man eine 2x3 Stiftleiste, die auf die Vorderseite (wie aufgedruckt) gelötet wird.

Will man gar keinen ISP zur Kopplung verwenden, kann man einfach die 3 rechten Lötpunkte, plus dem WD Reset von dem ISP verwenden. Dabei gilt 

| Pin   | Bedeutung |
| ----- | --------- |
| Pin 2 | +5V       |
| Pin 4 | Hearbeat  |
| Pin 5 | WD_Reset  |
| Pin 6 | GND       |



## NoSMD Variante

Hier mal die normale Variante:

 <img src="./images/noSMD_front.jpg" alt="noSMD_front" style="zoom: 33%;" /><img src="./images/noSMD_back.jpg" alt="noSMD_back" style="zoom: 33%;" />

Aufgesetzt auf meine Tonnenpumpe sieht dass so aus:

![noSMD_Tonnenpumpe.jpg](./images/noSMD_Tonnenpumpe.jpg)

Verwendet werden hier 2 Low-Current 3mm LEDs.

BOM:

| Anzahl | Bauteil                 | Gehäuse |
| ------ | ----------------------- | ------- |
| 1      | ATTiny85                | DIP8    |
| 1      | IC Sockel               | DIP8    |
| 2      | 1k5 Metallschicht       |         |
| 1      | 10k Metallschicht       |         |
| 2      | 100nF                   | RM2.54  |
| 1      | 3mm LED LowCurrent grün |         |
| 1      | 3mm LED LowCurrent rot  |         |

## SMD Variante

Bestückt sieht das ganze so aus:

<img src="./images/pcb_front_equ.jpg" alt="pcb_front_equ" style="zoom: 33%;" /><img src="./images/pcb_back_equ.jpg" alt="pcb_back_equ" style="zoom: 33%;" />



Und auf die Tonnenpumpeplatine aufgesteckt:
![tonnenpumpe_equipped](./images/tonnenpumpe_equipped.jpg)



| Anzahl | Bauteil               | Gehäuse |
| ------ | --------------------- | ------- |
| 1      | SMD ATTiny85          | SOIC-8  |
| 2      | SMD 1k5               | 1206    |
| 1      | SMD 10k Metallschicht | 1206    |
| 2      | SMD 100nF             | 1206    |
| 1      | SMD LED grün          | 1206    |
| 1      | SMD LED rot           | 1206    |

# Software

Die Software für den WD findet ihr hier in dem Github Repo: https://github.com/willie68/TinyWD

Die Software auf der Gegenseiten ist recht einfach. Man mus nur immer einen der ISP Pins als Ausgang definieren und in regelmäßigen Abständen diesen Pin toggeln.

das geht recht einfach mit `digitalWrite(pin, !digitalRead(pin))` 

Bei meiner Tonnenpumpe musste ich allerdings etwas anders vorgehen, da der Pin für den Heartbeat bereits anderweitig (LED Pumpe) vergeben war. Also gebe ich hier ca. jede Sekunde einen kurzen Implus (ca 10ms) auf den Pin. Um den Pin Status nicht zu verlieren, speichere ich mir diesen einmal vorher ab.

Wenn man den TinyWD aber von vornherein berücksichtigt, erspart man sich diesen zusätzlichen Aufwand.    

```c++
unsigned long saved = 0; 
void heartbeat() { 
  if (millis() > saved) { 
    saved = millis() + 1000; 
    bool tmp = digitalRead(LED_PUMP); 
    digitalWrite(LED_PUMP, !tmp); 
    delay(10); 
    digitalWrite(LED_PUMP, tmp); 
  } 
} 
```

