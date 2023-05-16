# Sistema de Alarma IoT

![IMG_0013](https://github.com/ardumiguel/sistemadealarmaiot/assets/133792399/d6731c86-4e20-4939-97ae-ad4bdeb8afc9)

Proyecto de Miguel Rodríguez, estudiante de 1º Bachillerato de ciencias y tecnología, presentado en la Feria Andaluza de Tecnología 2023 y en la 25º Feria de la Ciencia del Parque de las Ciencias

El sistema de alarma IoT se compone de tres módulos inalámbricos más el módulo principal. Cada uno de ellos cuenta con un ESP32 que, a través del protocolo de comunicación inalámbrica “ESP-now”, es capaz de comunicarse con el resto de módulos. Estos son: módulo de cámara con sensor de movimiento, sensor inalámbrico de puerta o ventana y sensor inalámbrico de movimiento. Cabe destacar que estos dos últimos son tan eficientes energéticamente hablando que su batería es capaz de durar más de un año con una carga completa.

El funcionamiento es simple e intuitivo. La alarma puede ser activada vía Telegram, que es la plataforma mediante la cual el sistema notifica al usuario y le envía fotos, o bien manualmente desde el módulo principal. Una vez el sistema está activo, al detectar algún movimiento o la apertura de una puerta, notifica de inmediato al usuario indicando el motivo de la alerta y acompañando el mensaje con una fotografía. Para desactivar el sistema, el usuario puede colocar su dedo (previamente registrado) en el lector de huellas incorporado en el módulo principal o en su defecto, enviar un comando por Telegram. El usuario también puede solicitar una fotografía o información acerca del sistema (niveles de batería, estado del sistema, estado de las puertas) en cualquier momento a través de Telegram.
