# main.py
import sys
from PyQt5.QtWidgets import QApplication
from main_window import ModernSTM32Terminal


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = ModernSTM32Terminal()
    window.show()
    sys.exit(app.exec_())
