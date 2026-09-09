import csv
import time
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
                             QPushButton, QLabel, QComboBox, QFrame, QToolBar,
                             QDoubleSpinBox, QMessageBox, QTableWidget, QTableWidgetItem, QFileDialog, QHeaderView)
from PyQt5.QtCore import Qt, QTimer
import pyqtgraph as pg


from config import (COLOR_BG_DARK, COLOR_CARD_BG, COLOR_TEXT_MAIN, COLOR_TEXT_DIM,
                    COLOR_NEON_GREEN, COLOR_NEON_ORANGE, COLOR_NEON_BLUE, COLOR_NEON_RED)
from serial_worker import SerialReaderThread
from widgets import LargeGlowingLed




class ModernSTM32Terminal(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ser = None
        self.reader_thread = None
        self.temp_history = []
        self.press_history = []
        # Eşik düzenleme modunda mıyız? (State Flag)
        self.is_editing_thresholds = False
        self.last_packet_time = 0  # Watchdog için zaman takibi
        self.initUI()


        # Haberleşme Timeout (Kablo Çekilme) Denetleyici Timer
        self.watchdog_timer = QTimer(self)
        self.watchdog_timer.timeout.connect(self.check_connection_timeout)
        self.watchdog_timer.start(1000) # Her 500 ms'de bir kontrol et


    def initUI(self):
        self.setWindowTitle('STM32L073Z TFT/I2C/ADC Industrial HMI Terminal')
        self.setGeometry(50, 50, 1360, 850)
        self.setStyleSheet(f"QMainWindow {{ background-color: {COLOR_BG_DARK}; }}")


        # TOOLBAR
        toolbar = QToolBar("Main Toolbar")
        toolbar.setStyleSheet(f"""
            QToolBar {{ background-color: {COLOR_CARD_BG}; padding: 8px; border-bottom: 2px solid #45475A; }}
            QLabel {{ color: {COLOR_TEXT_MAIN}; font-weight: bold; font-size: 13px; }}
            QComboBox {{ background-color: #11111B; color: {COLOR_NEON_GREEN}; padding: 6px; border-radius: 4px; border: 1px solid {COLOR_NEON_GREEN}; font-weight: bold; }}
            QPushButton {{ background-color: #313244; color: {COLOR_TEXT_MAIN}; font-weight: bold; padding: 6px 14px; border-radius: 4px; border: 1px solid #585B70; }}
            QPushButton:hover {{ background-color: #45475A; border-color: {COLOR_NEON_BLUE}; }}
        """)
        self.addToolBar(toolbar)


        toolbar.addWidget(QLabel(" COM Port: "))
        self.combo_ports = QComboBox()
        self.combo_ports.setMinimumWidth(130)
        toolbar.addWidget(self.combo_ports)


        btn_refresh = QPushButton("🔄 Yenile")
        btn_refresh.clicked.connect(self.refresh_ports)
        toolbar.addWidget(btn_refresh)


        toolbar.addSeparator()


        self.btn_connect = QPushButton("⚡ Bağlan")
        self.btn_connect.setStyleSheet(f"QPushButton {{ background-color: {COLOR_NEON_BLUE}; color: #000; font-weight: bold; }}")
        self.btn_connect.clicked.connect(self.toggle_connection)
        toolbar.addWidget(self.btn_connect)


        # MAIN LAYOUT
        main_widget = QWidget()
        main_layout = QHBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)


        # SOL PANEL
        left_panel = QFrame()
        left_panel.setFixedWidth(280)
        left_panel.setStyleSheet(f"background-color: {COLOR_CARD_BG}; border-radius: 10px; border: 1px solid #313244;")
        left_layout = QVBoxLayout()
        left_panel.setLayout(left_layout)


        thresh_title = QLabel("EŞİK DEĞER AYARLARI")
        thresh_title.setStyleSheet(f"color: {COLOR_NEON_GREEN}; font-weight: bold; font-size: 12px; letter-spacing: 1px;")
        left_layout.addWidget(thresh_title)
        left_layout.addSpacing(5)


        lbl_temp_limit = QLabel("Max Sıcaklık (°C):")
        lbl_temp_limit.setStyleSheet(f"color: {COLOR_NEON_ORANGE}; font-size: 12px; font-weight: bold;")
        left_layout.addWidget(lbl_temp_limit)
       
        self.spin_temp_max = QDoubleSpinBox()
        self.spin_temp_max.setRange(0, 100)
        self.spin_temp_max.setValue(35.0)
        self.spin_temp_max.setStyleSheet(f"background-color: #11111B; color: {COLOR_NEON_ORANGE}; padding: 6px; font-weight: bold; font-size: 14px; border: 1px solid #45475A; border-radius: 4px;")
        left_layout.addWidget(self.spin_temp_max)


        left_layout.addSpacing(5)


        lbl_press_limit = QLabel("Max Basınç (hPa):")
        lbl_press_limit.setStyleSheet(f"color: {COLOR_NEON_BLUE}; font-size: 12px; font-weight: bold;")
        left_layout.addWidget(lbl_press_limit)
       
        self.spin_press_max = QDoubleSpinBox()
        self.spin_press_max.setRange(800, 1300)
        self.spin_press_max.setValue(1050.0)
        self.spin_press_max.setStyleSheet(f"background-color: #11111B; color: {COLOR_NEON_BLUE}; padding: 6px; font-weight: bold; font-size: 14px; border: 1px solid #45475A; border-radius: 4px;")
        left_layout.addWidget(self.spin_press_max)


        # SpinBox Ayarları (0.5 adım hassasiyeti)
        self.spin_temp_max.setSingleStep(0.5)


        # "Eşikleri Değiştir" / "Düzenle" Butonu
        self.btn_edit_thresh = QPushButton("✏️ Eşikleri Düzenle")
        self.btn_edit_thresh.setStyleSheet("background-color: #45475A; color: #FFF; font-weight: bold; padding: 6px;")
        self.btn_edit_thresh.clicked.connect(self.enable_edit_mode)
        left_layout.addWidget(self.btn_edit_thresh)


       # --- Eşikleri STM32'ye Yaz Butonu ---
        self.btn_set_thresh = QPushButton("📤 Eşikleri STM32'ye Yaz")
        self.btn_set_thresh.setCursor(Qt.PointingHandCursor)
        self.btn_set_thresh.setStyleSheet("""
            QPushButton {
                background-color: #89B4FA;
                color: #11111B;
                font-weight: bold;
                font-size: 13px;
                padding: 8px;
                border-radius: 6px;
                border: none;
            }
            QPushButton:hover {
                background-color: #B4BEFE;
            }
        """)
        self.btn_set_thresh.clicked.connect(self.send_thresholds)
        left_layout.addWidget(self.btn_set_thresh)


        # --- Geribildirim Etiketi (Feedback Label) ---
        self.lbl_feedback = QLabel("")
        self.lbl_feedback.setAlignment(Qt.AlignCenter)
        self.lbl_feedback.setStyleSheet("font-weight: bold; font-size: 12px; min-height: 20px;")
        left_layout.addWidget(self.lbl_feedback)


        left_layout.addSpacing(10)


        # SpinBox'ları başlangıçta kilitli tutalım ki yanlışlıkla değişmesin
        self.spin_temp_max.setEnabled(False)
        self.spin_press_max.setEnabled(False)
       


        led_title = QLabel("DONANIM LED DURUMLARI")
        led_title.setStyleSheet(f"color: {COLOR_NEON_GREEN}; font-weight: bold; font-size: 12px; letter-spacing: 1px;")
        left_layout.addWidget(led_title)
        left_layout.addSpacing(5)


        self.led_conn = LargeGlowingLed("LD1: COM BAGLANTISI", COLOR_NEON_GREEN)
        self.led_temp = LargeGlowingLed("LD2: SICAKLIK ALARM", COLOR_NEON_ORANGE)
        self.led_emerg = LargeGlowingLed("LD3: ACIL DURUM", COLOR_NEON_RED)
        self.led_press = LargeGlowingLed("LD4: BASINC ALARM", COLOR_NEON_BLUE)


        left_layout.addWidget(self.led_conn)
        left_layout.addWidget(self.led_temp)
        left_layout.addWidget(self.led_emerg)
        left_layout.addWidget(self.led_press)


        main_layout.addWidget(left_panel)


        # SAĞ PANEL
        right_layout = QVBoxLayout()


        # METRİK KARTLARI
        metrics_layout = QHBoxLayout()


        # 1. Sıcaklık Kartı
        self.card_temp = QFrame()
        self.card_temp.setStyleSheet(f"background-color: {COLOR_CARD_BG}; border-radius: 8px; border-left: 5px solid {COLOR_NEON_ORANGE};")
        temp_box = QVBoxLayout()
        temp_box.addWidget(QLabel("STLM75 SICAKLIK (I2C)", styleSheet=f"color: {COLOR_TEXT_DIM}; font-size: 11px; font-weight: bold;"))
        self.lbl_temp_val = QLabel("--.- °C")
        self.lbl_temp_val.setStyleSheet(f"color: {COLOR_NEON_ORANGE}; font-size: 30px; font-weight: bold;")
        temp_box.addWidget(self.lbl_temp_val)
        self.card_temp.setLayout(temp_box)


        # 2. Basınç Kartı
        self.card_press = QFrame()
        self.card_press.setStyleSheet(f"background-color: {COLOR_CARD_BG}; border-radius: 8px; border-left: 5px solid {COLOR_NEON_BLUE};")
        press_box = QVBoxLayout()
        press_box.addWidget(QLabel("ANALOG BASINÇ SENSÖRÜ (ADC)", styleSheet=f"color: {COLOR_TEXT_DIM}; font-size: 11px; font-weight: bold;"))
        self.lbl_press_val = QLabel("----.-- hPa")
        self.lbl_press_val.setStyleSheet(f"color: {COLOR_NEON_BLUE}; font-size: 30px; font-weight: bold;")
        press_box.addWidget(self.lbl_press_val)
        self.card_press.setLayout(press_box)


        metrics_layout.addWidget(self.card_temp)
        metrics_layout.addWidget(self.card_press)
        right_layout.addLayout(metrics_layout)


        # GRAFİKLER
        graphs_layout = QHBoxLayout()


        self.plot_temp = pg.PlotWidget(title="Sıcaklık Değişim Grafiği")
        self.plot_temp.setBackground(COLOR_CARD_BG)
        self.plot_temp.showGrid(x=True, y=True, alpha=0.2)
        self.plot_temp.setLabel('left', 'Sıcaklık', units='°C')
        self.plot_temp.setLabel('bottom', 'Örnek Sayısı / Zaman')
        self.temp_curve = self.plot_temp.plot(pen=pg.mkPen(COLOR_NEON_ORANGE, width=2))


        self.plot_press = pg.PlotWidget(title="Basınç Değişim Grafiği")
        self.plot_press.setBackground(COLOR_CARD_BG)
        self.plot_press.showGrid(x=True, y=True, alpha=0.2)
        self.plot_press.setLabel('left', 'Basınç', units='hPa')
        self.plot_press.setLabel('bottom', 'Örnek Sayısı / Zaman')
        self.press_curve = self.plot_press.plot(pen=pg.mkPen(COLOR_NEON_BLUE, width=2))


        graphs_layout.addWidget(self.plot_temp)
        graphs_layout.addWidget(self.plot_press)
        right_layout.addLayout(graphs_layout, stretch=2)


        # EEPROM Tablosu
        eeprom_frame = QFrame()
        eeprom_frame.setStyleSheet(f"background-color: {COLOR_CARD_BG}; border-radius: 8px;")
        eeprom_box = QVBoxLayout()
       
        tbl_header_layout = QHBoxLayout()
        lbl_tbl = QLabel("EEPROM GEÇMİŞ LOGLARI")
        lbl_tbl.setStyleSheet(f"color: {COLOR_TEXT_MAIN}; font-weight: bold; font-size: 12px;")
        tbl_header_layout.addWidget(lbl_tbl)


        btn_export_csv = QPushButton("💾 CSV'ye Kaydet")
        btn_export_csv.setStyleSheet(f"background-color: #313244; color: {COLOR_NEON_GREEN}; font-weight: bold; padding: 4px 8px; border: 1px solid {COLOR_NEON_GREEN};")
        btn_export_csv.clicked.connect(self.export_eeprom_to_csv)
        tbl_header_layout.addWidget(btn_export_csv)
       
        btn_read_eeprom = QPushButton("📥 EEPROM Dökümünü İste")
        btn_read_eeprom.setStyleSheet(f"background-color: #313244; color: {COLOR_TEXT_MAIN}; font-weight: bold; padding: 4px 8px;")
        btn_read_eeprom.clicked.connect(self.request_eeprom)
        tbl_header_layout.addWidget(btn_read_eeprom)
        eeprom_box.addLayout(tbl_header_layout)


        self.table_logs = QTableWidget(0, 4)
        self.table_logs.setHorizontalHeaderLabels(["ID", "Zaman (RTC)", "Sıcaklık (°C)", "Basınç (hPa)"])
        self.table_logs.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table_logs.setStyleSheet(f"""
            QTableWidget {{ background-color: #11111B; color: {COLOR_TEXT_MAIN}; gridline-color: #313244; border: none; }}
            QHeaderView::section {{ background-color: #313244; color: {COLOR_TEXT_MAIN}; font-weight: bold; padding: 4px; border: none; }}
        """)
        eeprom_box.addWidget(self.table_logs)
        eeprom_frame.setLayout(eeprom_box)
        right_layout.addWidget(eeprom_frame, stretch=1)


        main_layout.addLayout(right_layout, stretch=1)


        self.refresh_ports()


    def disconnect_cleanly(self):
        if self.reader_thread:
            self.reader_thread.stop()
        if self.ser and self.ser.is_open:
            self.ser.close()


        self.btn_connect.setText("⚡ Bağlan")
        self.btn_connect.setStyleSheet(f"background-color: {COLOR_NEON_BLUE}; color: #000; font-weight: bold;")
       
        # Tüm LED'leri Söndür
        self.led_conn.set_state(False)
        self.led_temp.set_state(False)
        self.led_emerg.set_state(False)
        self.led_press.set_state(False)


    def check_connection_timeout(self):
        """Kablo çekildiğinde veya 2.5 saniye veri gelmediğinde LED'leri söndürür."""
        if self.ser and self.ser.is_open:
            if time.time() - self.last_packet_time > 1.5:
                # Veri akışı koptu!
                self.led_conn.set_state(False)
                self.led_temp.set_state(False)
                self.led_emerg.set_state(False)
                self.led_press.set_state(False)


    def refresh_ports(self):
        self.combo_ports.clear()
        ports = serial.tools.list_ports.comports()
        for p in ports:
            self.combo_ports.addItem(p.device)


    def toggle_connection(self):
        if self.ser is None or not self.ser.is_open:
            port = self.combo_ports.currentText()
            try:
                self.ser = serial.Serial(port, 115200, timeout=1)
                self.btn_connect.setText("🔴 Bağlantıyı Kes")
                self.btn_connect.setStyleSheet(f"background-color: {COLOR_NEON_RED}; color: #FFF; font-weight: bold;")
               
                self.led_conn.set_state(True)
                #Öself.led_emerg.set_state(True)


                self.reader_thread = SerialReaderThread(self.ser)
                self.reader_thread.telemetry_received.connect(self.on_telemetry_received)
                self.reader_thread.eeprom_line_received.connect(self.on_eeprom_line_received)
                self.reader_thread.start()


            except Exception as e:
                print(f"Bağlantı hatası: {e}")
        else:
            if self.reader_thread:
                self.reader_thread.stop()
            if self.ser and self.ser.is_open:
                self.ser.close()


            self.btn_connect.setText("⚡ Bağlan")
            self.btn_connect.setStyleSheet(f"background-color: {COLOR_NEON_BLUE}; color: #000; font-weight: bold;")
           
            self.led_conn.set_state(False)
            self.led_temp.set_state(False)
            self.led_emerg.set_state(False)
            self.led_press.set_state(False)


    def enable_edit_mode(self):
        """Kullanıcı 'Düzenle' butonuna bastığında çalışır."""
        self.is_editing_thresholds = True
       
        # SpinBox'ların kilidini aç
        self.spin_temp_max.setEnabled(True)
        self.spin_press_max.setEnabled(True)
       
        # Buton durumlarını güncelle
        self.btn_set_thresh.setEnabled(True)
        self.btn_set_thresh.setStyleSheet("background-color: #89B4FA; color: #11111B; font-weight: bold; padding: 8px;")
       
        self.btn_edit_thresh.setText("❌ Düzenlemeyi İptal Et")
        self.btn_edit_thresh.clicked.disconnect()
        self.btn_edit_thresh.clicked.connect(self.cancel_edit_mode)


    def cancel_edit_mode(self):
        """Düzenleme iptal edilirse çalışır."""
        self.is_editing_thresholds = False
       
        self.spin_temp_max.setEnabled(False)
        self.spin_press_max.setEnabled(False)
       
        self.btn_set_thresh.setEnabled(False)
        self.btn_set_thresh.setStyleSheet("background-color: #313244; color: #7F849C; font-weight: bold; padding: 8px;")
       
        self.btn_edit_thresh.setText("✏️ Eşikleri Düzenle")
        self.btn_edit_thresh.clicked.disconnect()
        self.btn_edit_thresh.clicked.connect(self.enable_edit_mode)


    # Parametre İmzası ve Eşik Senkronizasyonu ---
    def on_telemetry_received(self, temp: float, press: float, temp_thresh: float, press_thresh: float, is_emergency: bool):
        self.last_packet_time = time.time() # Zamanı tazele


        # 1. COM Bağlantı LED'i (Sürekli Sabit Yanar)
        self.led_conn.set_state(True)


        self.lbl_temp_val.setText(f"{temp:.1f} °C")
        self.lbl_press_val.setText(f"{press:.2f} hPa")


        # 2. Eşik SpinBox'ları SADECE DÜZENLEME MODUNDA DEĞİLSEK güncellenir!
        if not self.is_editing_thresholds:
            self.spin_temp_max.blockSignals(True)
            self.spin_temp_max.setValue(temp_thresh)
            self.spin_temp_max.blockSignals(False)


            self.spin_press_max.blockSignals(True)
            self.spin_press_max.setValue(press_thresh)
            self.spin_press_max.blockSignals(False)


        # Donanım LED Durumlarını Aktif Eşiklere Göre Yak/Söndür
        if temp >= temp_thresh:
            self.led_temp.set_state(True)
        else:
            self.led_temp.set_state(False)


        if press >= press_thresh:
            self.led_press.set_state(True)
        else:
            self.led_press.set_state(False)


        # ACİL DURUM LED'ini Gelen Bayta Göre Yak/Söndür
        self.led_emerg.set_state(is_emergency)


        # Grafik verilerini güncelle
        self.temp_history.append(temp)
        self.press_history.append(press)
        if len(self.temp_history) > 50:
            self.temp_history.pop(0)
            self.press_history.pop(0)


        self.temp_curve.setData(self.temp_history)
        self.press_curve.setData(self.press_history)


    def request_eeprom(self):
        self.table_logs.setRowCount(0)
        self.send_cmd("eeprom\r\n")


    def send_cmd(self, cmd_data):
        """Metin (str) veya ham bayt (bytes/bytearray) kabul eder ve UART'a yollar."""
        if self.ser and self.ser.is_open:
            if isinstance(cmd_data, str):
                self.ser.write(cmd_data.encode('utf-8'))
            elif isinstance(cmd_data, (bytes, bytearray)):
                self.ser.write(cmd_data)


    def on_eeprom_line_received(self, line: str):
        parts = line.split(',')
        if len(parts) == 4 and not line.startswith("---") and not line.startswith("ID"):
            row = self.table_logs.rowCount()
            self.table_logs.insertRow(row)
            self.table_logs.setItem(row, 0, QTableWidgetItem(parts[0].strip()))
            self.table_logs.setItem(row, 1, QTableWidgetItem(parts[1].strip()))
            self.table_logs.setItem(row, 2, QTableWidgetItem(parts[2].strip()))
            self.table_logs.setItem(row, 3, QTableWidgetItem(parts[3].strip()))


    def export_eeprom_to_csv(self):
        """Tablodaki verileri File Explorer açarak CSV dosyası olarak kaydeder."""
        if self.table_logs.rowCount() == 0:
            QMessageBox.warning(self, "Uyarı", "Kaydedilecek EEPROM verisi bulunamadı! Önce döküm isteyin.")
            return


        # Dosya Kaydetme Penceresini (File Explorer) Aç
        file_path, _ = QFileDialog.getSaveFileName(
            self,
            "EEPROM Verilerini CSV Olarak Kaydet",
            "eeprom_loglari.csv",
            "CSV Dosyaları (*.csv);;Tüm Dosyalar (*)"
        )


        # Kullanıcı iptal etmediyse dosyayı yaz
        if file_path:
            try:
                with open(file_path, mode='w', newline='', encoding='utf-8') as file:
                    writer = csv.writer(file)
                   
                    # 1. Başlıkları Yaz (ID, Zaman (RTC), Sıcaklık (°C), Basınç (hPa))
                    headers = []
                    for col in range(self.table_logs.columnCount()):
                        headers.append(self.table_logs.horizontalHeaderItem(col).text())
                    writer.writerow(headers)


                    # 2. Satırdaki Verileri Yaz
                    for row in range(self.table_logs.rowCount()):
                        row_data = []
                        for col in range(self.table_logs.columnCount()):
                            item = self.table_logs.item(row, col)
                            row_data.append(item.text() if item else "")
                        writer.writerow(row_data)


                QMessageBox.information(self, "Başarılı", f"EEPROM verileri başarıyla kaydedildi:\n{file_path}")
            except Exception as e:
                QMessageBox.critical(self, "Hata", f"Dosya kaydedilirken bir hata oluştu:\n{e}")


    def send_thresholds(self):
        if self.ser is None or not self.ser.is_open:
            self.lbl_feedback.setText("⚠️ HATA: Önce Bağlanın!")
            self.lbl_feedback.setStyleSheet(f"color: {COLOR_NEON_RED}; font-weight: bold; font-size: 12px;")
            QTimer.singleShot(3000, lambda: self.lbl_feedback.setText(""))
            return


        t_max = int(self.spin_temp_max.value() * 2.0) # 0.5C hassasiyet (Örn: 26.5 -> 53)
        p_max = int(self.spin_press_max.value() - 900)


        pkt = bytearray([0xAA, 0x55, 0x04, t_max, p_max, 0x0D])
        self.send_cmd(pkt)
       
        self.lbl_feedback.setText("✓ STM32'ye Yazıldı!")
        self.lbl_feedback.setStyleSheet(f"color: {COLOR_NEON_GREEN}; font-weight: bold; font-size: 12px;")
        QTimer.singleShot(3000, lambda: self.lbl_feedback.setText(""))


        # Kaydettikten sonra tekrar normal izleme moduna dön
        self.cancel_edit_mode()


    def closeEvent(self, event):
        if self.reader_thread:
            self.reader_thread.stop()
        if self.ser and self.ser.is_open:
            self.ser.close()
        event.accept()
