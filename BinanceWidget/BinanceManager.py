from threading import Lock

from binance.client import Client as BinanceClient
from binance.websockets import BinanceSocketManager
from twisted.internet import reactor

from SymbolTickerValues import SYMBOL_TICKER_VALUES


class CBinanceManager:

    def __init__(self):
        self._binance_client = BinanceClient()
        self._binance_socket_manager = BinanceSocketManager(self._binance_client)
        self._connection_keys = []
        self._binance_response_data = {}
        self._binance_data_lock = Lock()

    def get_data(self):
        with self._binance_data_lock:
            return self._binance_response_data.copy()

    def register_symbol(self, symbol):
        self._binance_response_data[symbol] = {SYMBOL_TICKER_VALUES['event_time']: 0}
        self._connection_keys.append(
            self._binance_socket_manager.start_symbol_ticker_socket(symbol, self._trade_history))

    def start(self):
        self._binance_socket_manager.start()

    def stop(self):
        self._terminate_web_sockets()

    def _terminate_web_sockets(self):
        for key in self._connection_keys:
            self._binance_socket_manager.stop_socket(key)

        reactor.stop()

    def _trade_history(self, message):
        if SYMBOL_TICKER_VALUES['event_type'] in message and SYMBOL_TICKER_VALUES['symbol'] in message:
            if message[SYMBOL_TICKER_VALUES['event_type']] == 'error':
                print("SERVER ERROR: for " + message[SYMBOL_TICKER_VALUES['symbol']])
            else:
                with self._binance_data_lock:
                    self._binance_response_data[message[SYMBOL_TICKER_VALUES['symbol']]] = message