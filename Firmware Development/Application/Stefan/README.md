# ESP32 Flashing Instructions

Follow these steps to flash your ESP32 with the required binaries and data:

## 1. Download the Flash Download Tool

- Download the ESP32 Flash Download Tool from the [official source](https://www.esp32.com/download/file.php?id=103).

## 2. Launch the Flash Download Tool

- Open the tool and select **ESP32** from the dropdown menu.

![Select ESP32](https://github.com/user-attachments/assets/42e27688-0ce6-4e98-af52-275fd383c747)

## 3. Configure the Binary

1. **Browse** and select the binary file to flash: `Stefan.ino.merged.bin`.
2. Set the flash address to `0x0`.

![Configure Binary](https://github.com/user-attachments/assets/c0335479-bacb-482b-9639-5c01df14a97a)

## 4. Flash the Binary

- Click **Start** to begin the flashing process.

---

## 5. Flash the Data Folder with Arduino IDE

1. Launch **Arduino IDE 1.8**.

![Arduino IDE](https://github.com/user-attachments/assets/58540550-bba5-4f69-84fe-79b7d5aaa3a0)

2. Select **SPIFFS** as the partition scheme.
3. Flash the data folder.

![Select SPIFFS](https://github.com/user-attachments/assets/01233574-6c4b-4869-af73-ca5b4b2bedfb)

---

