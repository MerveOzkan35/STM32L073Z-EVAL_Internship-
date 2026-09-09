# widgets.py
from PyQt5.QtWidgets import QFrame, QVBoxLayout, QLabel
from PyQt5.QtCore import Qt


from config import COLOR_NEON_GREEN, COLOR_TEXT_MAIN, COLOR_OFF_LED

class LargeGlowingLed(QFrame):
    def __init__(self, label_text, neon_color=COLOR_NEON_GREEN):
        super().__init__()
        self.neon_color = neon_color
        self.is_on = False


        layout = QVBoxLayout()
        layout.setContentsMargins(5, 5, 5, 5)
        layout.setAlignment(Qt.AlignCenter)


        self.label = QLabel(label_text)
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setStyleSheet(f"color: {COLOR_TEXT_MAIN}; font-weight: bold; font-size: 11px;")


        self.led_circle = QFrame()
        self.led_circle.setFixedSize(45, 45)


        layout.addWidget(self.label)
        layout.addSpacing(4)
        layout.addWidget(self.led_circle, alignment=Qt.AlignCenter)
        self.setLayout(layout)
        self.set_state(False)


    def set_state(self, state: bool):
        self.is_on = state
        if self.is_on:
            self.led_circle.setStyleSheet(f"""
                background-color: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0 #FFFFFF, stop:0.35 {self.neon_color}, stop:1.0 #000000);
                border-radius: 22px;
                border: 3px solid {self.neon_color};
            """)
        else:
            self.led_circle.setStyleSheet(f"""
                background-color: {COLOR_OFF_LED};
                border-radius: 22px;
                border: 2px solid #45475A;
            """)
