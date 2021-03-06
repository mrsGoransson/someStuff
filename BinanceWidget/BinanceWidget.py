import sys
from threading import Thread

from AlertSystem import CAlertSystem
from BinanceManager import CBinanceManager
from FileWatcher import CFileWatcher
from GraphicalWindow import CGraphicalWindow
from SymbolLoader import CSymbolLoader
from TextMessageHandler import CTextMessageHandler
from Timer import CTimer


def _on_symbols_changed():
    binance_manager.reset()
    symbol_loader.reload()
    _register_symbols()
    binance_manager.start()


def _register_symbols():
    loaded_symbols = symbol_loader.get_loaded_symbols
    for loaded_symbol in loaded_symbols:
        binance_manager.register_symbol(loaded_symbol['symbol'])
        alert_system.register_symbol(loaded_symbol['symbol'], loaded_symbol['value_up'], loaded_symbol['value_down'])


if __name__ == '__main__':
    print("Hello, don't close me unless you want to close the application!")
    timer = CTimer()
    timer.start()

    alerts_enabled = False
    alert_system = CAlertSystem(CTextMessageHandler(), alerts_enabled)

    binance_manager = CBinanceManager()
    symbol_loader = CSymbolLoader()
    file_watcher = CFileWatcher()

    _register_symbols()
    file_watcher.add_listener('on_modified', 'symbols.txt', _on_symbols_changed)
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
