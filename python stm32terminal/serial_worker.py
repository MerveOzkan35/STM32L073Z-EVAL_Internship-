# serial_worker.py
from PyQt5.QtCore import QThread, pyqtSignal


class SerialReaderThread(QThread):
    telemetry_received = pyqtSignal(float, float, float, float, bool)
    eeprom_line_received = pyqtSignal(str)
    error_occurred = pyqtSignal(str)


    def __init__(self, serial_port):
        super().__init__()
        self.ser = serial_port
        self.running = True


    def run(self):
        buffer = bytearray()
        while self.running and self.ser and self.ser.is_open:
            try:
                if self.ser.in_waiting > 0:
                    raw_bytes = self.ser.read(self.ser.in_waiting)
                    buffer.extend(raw_bytes)


                    while len(buffer) >= 13:
                        # 0xAA 0x55 başlığını ara
                        header_index = -1
                        for i in range(len(buffer) - 1):
                            if buffer[i] == 0xAA and buffer[i+1] == 0x55:
                                header_index = i
                                break


                        if header_index != -1:
                            if header_index > 0:
                                buffer = buffer[header_index:]


                            # 13 baytlık tam paket kontrolü
                            if len(buffer) >= 13:
                                if buffer[12] == 0x0D: # 13. indeks Footer (0x0D) kontrolü
                                    pkt = buffer[:13]
                                    buffer = buffer[13:]


                                    cmd = pkt[2]
                                    if cmd == 0x01: # Telemetri
                                        temp = float(pkt[3]) + (float(pkt[4]) / 10.0)
                                        press_scaled = (pkt[5] << 24) | (pkt[6] << 16) | (pkt[7] << 8) | pkt[8]
                                        press_actual = float(press_scaled) / 100.0
                                        temp_thresh = float(pkt[9]) / 2.0
                                        press_thresh = float(pkt[10]) + 900.0
                                        is_emergency = bool(pkt[11] == 0x01)


                                        self.telemetry_received.emit(temp, press_actual, temp_thresh, press_thresh, is_emergency)
                                else:
                                    # Footer uyuşmadıysa hatalı senkronizasyon, 1 bayt atla
                                    buffer.pop(0)
                            else:
                                break
                        else:
                            # String (EEPROM) verisi kontrolü
                            if b'\n' in buffer:
                                line_end = buffer.index(b'\n')
                                raw_line = buffer[:line_end + 1]
                                buffer = buffer[line_end + 1:]
                                try:
                                    line_str = raw_line.decode('utf-8', errors='ignore').strip()
                                    if line_str:
                                        self.eeprom_line_received.emit(line_str)
                                except Exception:
                                    pass
                            else:
                                break


            except Exception as e:
                self.error_occurred.emit(str(e))
                break
            self.msleep(10)


    def stop(self):
        self.running = False
        self.wait()


