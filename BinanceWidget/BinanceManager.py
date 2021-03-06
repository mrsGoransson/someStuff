from threading import Lock

from binance.client import Client as BinanceClient
from binance.websockets import BinanceSocketManager
from twisted.internet import reactor

from SymbolTickerValues import SYMBOL_TICKER_VALUES


class CBinanceManager:

    def __init__(self):
        self._binance_client = BinanceClient()
        self._binance_socket_manager = BinanceSocketManager(self._binance_client)
        self._binance_socket_manager.start()
        self._binance_response_data = {}
        self._binance_data_lock = Lock()
        self._connection_keys = []
        self._connection_key_postfix = '@ticker'

    def get_data(self):
        with self._binance_data_lock:
            return self._binance_response_data.copy()

    def register_symbol(self, symbol):
        self._binance_response_data[symbol] = {}
        self._connection_keys.append(symbol.lower() + self._connection_key_postfix)

    def start(self):
        self._binance_socket_manager.start_multiplex_socket(self._connection_keys, self._trade_history)

    def stop(self):
        self._binance_socket_manager.close()
        reactor.stop()

    def reset(self):
        self._binance_socket_manager.close()
        with self._binance_data_lock:
            self._binance_response_data = {}

    def _trade_history(self, message):
        if 'data' in message:
            if SYMBOL_TICKER_VALUES['event_type'] in message['data'] \
                    and SYMBOL_TICKER_VALUES['symbol'] in message['data']:
                if message['data'][SYMBOL_TICKER_VALUES['event_type']] == 'error':
                    print("SERVER ERROR: for " + message['data'][SYMBOL_TICKER_VALUES['symbol']])
                else:
                    with self._binance_data_lock:
                        self._binance_response_data[message['data'][SYMBOL_TICKER_VALUES['symbol']]] = message['data']
