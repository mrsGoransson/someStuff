import sys
from os import path
from threading import Thread
from distutils.util import strtobool

from AlertSystem import CAlertSystem
from BinanceManager import CBinanceManager
from GraphicalWindow import CGraphicalWindow
import SymbolLineResolver
from TextMessageHandler import CTextMessageHandler
from Timer import CTimer


if __name__ == '__main__':
    print("Hello, don't close me unless you want to close the application!")
    timer = CTimer()
    timer.start()

    symbols_list_file_name = 'symbols.txt'
    if path.isfile(symbols_list_file_name):
        with open(symbols_list_file_name) as symbols_file:
            symbol_file_lines = symbols_file.readlines()
    else:
        print("ERROR: Could not find file with symbols to load: " + symbols_list_file_name)
        input("Press 'enter' to exit...")
        sys.exit(1)

    if len(symbol_file_lines) < 1:
        print("ERROR: Failed to find symbols to load, file appear to be empty: " + symbols_list_file_name)
        input("Press 'enter' to exit...")
        sys.exit(1)

    alerts_enabled = True
    if len(sys.argv) >= 2:
        alerts_enabled = bool(strtobool(sys.argv[1]))

    alert_system = CAlertSystem(CTextMessageHandler(), alerts_enabled)
    binance_manager = CBinanceManager()
    for line in symbol_file_lines:
        [symbol_to_load, warning_value_up, warning_value_down] = SymbolLineResolver.resolve_symbol_line(line)
        if len(symbol_to_load) < 1:
            continue

        binance_manager.register_symbol(symbol_to_load)
        alert_system.register_symbol(symbol_to_load, warning_value_up, warning_value_down)

    binance_manager.start()
    graphical_window = CGraphicalWindow(binance_manager, alert_system)
    graphical_window_thread = Thread(target=graphical_window.start_and_run, daemon=True)
    graphical_window_thread.start()

    while True:
        if not graphical_window_thread.is_alive():
            binance_manager.stop()
            sys.exit()

        alert_system.update(binance_manager.get_data())
        timer.update()
