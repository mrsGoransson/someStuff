import sys
import PySimpleGUI
import threading

from os import path
from binance.client import Client
from binance.websockets import BinanceSocketManager
from twisted.internet import reactor

from SymbolTickerValues import SYMBOL_TICKER_VALUES


def _create_and_run_graphical_window():
    window = _create_graphical_window()
    previous_response_data = {}
    new_binance_response_data = {}

    while True:
        event, values = window.read(timeout=10)
        if event == "Close" or event == PySimpleGUI.WIN_CLOSED:
            break

        if len(previous_response_data) > 0:
            first_key = list(previous_response_data.keys())[0]
            if new_binance_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']] > \
                    previous_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']]:
                window['_table_'].update(values=_get_new_data_for_window_table(new_binance_response_data))

        with threading_lock:
            previous_response_data = new_binance_response_data.copy()
            new_binance_response_data = binance_response_data.copy()

        window.refresh()

    window.close()


def _create_client():
    new_client = Client()
    return new_client


def _create_graphical_window():
    PySimpleGUI.theme('Topanga')
    data = ['Loading...']
    header_list = ['Currency ', 'Weighted Average Price', 'High Price   ', 'Low Price    ', 'Total Nr of Trades']
    layout = [
        [PySimpleGUI.Table(values=data, max_col_width=25,
                           background_color='black',
                           auto_size_columns=True,
                           justification='right', alternating_row_color='grey',
                           key='_table_', headings=header_list)],
        [PySimpleGUI.Text("Not your keys, not your coins!")],
        [PySimpleGUI.Button("Close")]]

    return PySimpleGUI.Window(title="Crypto Check v1", layout=layout, debugger_enabled=False, finalize=True)


def _create_socket_manager():
    new_socket_manager = BinanceSocketManager(client)
    return new_socket_manager


def _get_new_data_for_window_table(latest_response_data):
    new_data_to_render = []
    for element in latest_response_data:
        if latest_response_data[element][SYMBOL_TICKER_VALUES['event_time']] > 0:
            new_data_to_render.append([element,
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['weighted_average_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['high_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['low_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['total_number_of_trades']]])

        else:
            new_data_to_render.append([element, 0, 0, 0, 0])
    return new_data_to_render


def _start_trade_history_web_socket_for_symbol(symbol_to_start):
    connection_keys.append(socket_manager.start_symbol_ticker_socket(symbol_to_start, _trade_history))


def _terminate_web_sockets():
    for key in connection_keys:
        socket_manager.stop_socket(key)

    reactor.stop()


def _trade_history(message):
    if message['e'] == 'error':
        print("SERVER ERROR: for " + message['s'])
    else:
        with threading_lock:
            binance_response_data[message['s']] = message


if __name__ == '__main__':
    print("Hello, don't close me unless you want to close Binance Widget")

    symbols_list_file_name = 'symbols.txt'
    if path.isfile(symbols_list_file_name):
        with open(symbols_list_file_name) as symbols_file:
            symbols = symbols_file.readlines()
    else:
        print("ERROR: Could not find file with symbols to load")
        input("Press 'enter' to exit...")
        sys.exit(1)

    if len(symbols) < 1:
        print("ERROR: Failed to find symbols to load, file appear to be empty")
        input("Press 'enter' to exit...")
        sys.exit(1)

    connection_keys = []
    binance_response_data = {}
    client = _create_client()
    socket_manager = _create_socket_manager()
    for symbol in symbols:
        if symbol.endswith('\n'):
            symbol = symbol[:-1]

        _start_trade_history_web_socket_for_symbol(symbol)
        binance_response_data[symbol] = {'E': 0}

    socket_manager.start()

    threading_lock = threading.Lock()
    graphical_window_thread = threading.Thread(target=_create_and_run_graphical_window, daemon=True)
    graphical_window_thread.start()

    while True:
        if not graphical_window_thread.is_alive():
            _terminate_web_sockets()
            sys.exit()
