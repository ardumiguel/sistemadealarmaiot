# Sistema de Alarma IoT


Proyecto de Miguel Rodríguez, estudiante de 1º Bachillerato de ciencias y tecnología, presentado en la **Feria Andaluza de Tecnología 2023** y en la **25º Feria de la Ciencia** del Parque de las Ciencias.

![IMG_0013](https://github.com/ardumiguel/sistemadealarmaiot/assets/133792399/6366e4f0-d242-4616-a612-eaad69f976a9)

El sistema de alarma IoT se compone de cuatro módulos inalámbricos. Cada uno de ellos cuenta con un ESP32 que, a través del protocolo de comunicación inalámbrica “ESP-now”, es capaz de comunicarse con el resto de módulos. Estos son: sensor inalámbrico de puerta o ventana, panel de control, módulo de cámara con sensor de movimiento, y sensor inalámbrico de movimiento. Cabe destacar que los modulos de sensor de movimiento y de puerta o ventana son tan eficientes energéticamente en cuanto a términos energéticos que su duración de batería es de 18 meses.


## Características

- Control vía Telegram (notificaciones instantáneas, control desde cualquier parte del mundo)
- Lector óptico de huella dactilar
- Módulo de cámara con lente ultra gran angular
- Larga duración de la bateria de los módulos inalámbricos


## Funcionamiento
El funcionamiento está diseñado para que el manejo de la alarma por parte del usuario sea simple e intuitivo.

La alarma puede ser activada vía Telegram, que es la plataforma mediante la cual el sistema notifica al usuario y le envía fotos, o bien manualmente desde el módulo principal. Una vez el sistema está activo, al detectar algún movimiento o la apertura de una puerta, notifica de inmediato al usuario indicando el motivo de la alerta y acompañando el mensaje con una fotografía. 

Para desactivar el sistema, el usuario puede colocar su dedo (previamente registrado) en el lector de huellas incorporado en el módulo principal o en su defecto, enviar un comando por Telegram. El usuario también puede solicitar una fotografía o información acerca del sistema (niveles de batería, estado del sistema, estado de las puertas) en cualquier momento a través de Telegram.
## Librerías necesarias

- [AsyncTelegram2](https://github.com/cotestatnt/AsyncTelegram2)
- [Keypad](https://github.com/Chris--A/Keypad)
- [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [Adafruit-Fingerprint-Sensor-Library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library)


## Autor

- [@ardumiguel](https://github.com/ardumiguel)

