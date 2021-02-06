import sys
from os import path
from threading import Thread
from distutils.util import strtobool

import PySimpleGUI

from AlertSystem import CAlertSystem
from BinanceManager import CBinanceManager
from SymbolTickerValues import SYMBOL_TICKER_VALUES
from TextMessageHandler import CTextMessageHandler
from Timer import CTimer


def _create_and_run_graphical_window():
    window = _create_graphical_window()
    previous_response_data = {}
    new_binance_response_data = {}

    while True:
        event, values = window.read(timeout=10)
        if event == "_close" or event == PySimpleGUI.WIN_CLOSED:
            break
        elif event == '_toggle_alerts':
            alert_system.toggle_alerts_enabled()
            if alert_system.alerts_enabled():
                window['_toggle_alerts'].update(text='Disable Alert Messages', button_color=('black', 'orange'))
            else:
                window['_toggle_alerts'].update(text='Enable Alert Messages', button_color=('light goldenrod', 'green'))

        if len(previous_response_data) > 0:
            first_key = list(previous_response_data.keys())[0]
            if new_binance_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']] > \
                    previous_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']]:
                window['_table'].update(values=_get_new_data_for_window_table(new_binance_response_data))

        previous_response_data = new_binance_response_data.copy()
        new_binance_response_data = binance_manager.get_data()

        window.refresh()

    window.close()


def _create_graphical_window():
    PySimpleGUI.theme('Topanga')
    data = ['Loading...']
    header_list = ['Currency ', 'Weighted Average Price', 'High Price   ', 'Low Price    ', 'Price Change %',
                   'Total Nr of Trades']

    layout = [
        [PySimpleGUI.Table(values=data, max_col_width=25,
                           background_color='black',
                           auto_size_columns=True,
                           justification='right', alternating_row_color='grey',
                           key='_table', headings=header_list)],
        [PySimpleGUI.Text("Not your keys, not your coins!")],
        [PySimpleGUI.Button(button_text="Close", key='_close'),
         PySimpleGUI.Button(button_text='Disable Alert Messages' if alert_system.alerts_enabled() else 'Enable Alert '
                                                                                                       'Messages',
                            button_color=('black', 'orange') if alert_system.alerts_enabled() else ('light goldenrod', 'green'),
                            key='_toggle_alerts')]]

    return PySimpleGUI.Window(title="Crypto Check v1.1", layout=layout, debugger_enabled=False, finalize=True)


def _get_new_data_for_window_table(latest_response_data):
    new_data_to_render = []
    for element in latest_response_data:
        if latest_response_data[element][SYMBOL_TICKER_VALUES['event_time']] > 0:
            new_data_to_render.append([element,
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['weighted_average_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['high_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['low_price']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['price_change_percent']],
                                       latest_response_data[element][SYMBOL_TICKER_VALUES['total_number_of_trades']]])

        else:
            new_data_to_render.append([element, 0, 0, 0, 0])
    return new_data_to_render


def _resolve_symbol_line(symbol_line):
    if symbol_line.endswith('\n'):
        symbol_line = symbol_line[:-1]
    if len(symbol_line) < 1:
        return ["", 0, 0]

    alert_value_down = 0
    alert_value_up = 0
    words_in_line = symbol_line.split()
    resolved_symbol = words_in_line[0]
    resolved_symbol = resolved_symbol.strip()
    if len(words_in_line) > 2:
        when_to_warn_down = words_in_line[2]
        try:
            alert_value_down = float(when_to_warn_down)
        except ValueError:
            print("Error: wrong format of lower alert value for " + resolved_symbol)
    if len(words_in_line) > 1:
        when_to_warn_up = words_in_line[1]
        try:
            alert_value_up = float(when_to_warn_up)
        except ValueError:
            print("Error: wrong format of upper warning value for " + resolved_symbol)

    return [resolved_symbol, alert_value_up, alert_value_down]


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
        [symbol_to_load, warning_value_up, warning_value_down] = _resolve_symbol_line(line)
        if len(symbol_to_load) < 1:
            continue

        binance_manager.register_symbol(symbol_to_load)
        alert_system.register_symbol(symbol_to_load, warning_value_up, warning_value_down)

    binance_manager.start()
    graphical_window_thread = Thread(target=_create_and_run_graphical_window, daemon=True)
    graphical_window_thread.start()

    while True:
        if not graphical_window_thread.is_alive():
            binance_manager.stop()
            sys.exit()

        alert_system.update(timer, binance_manager.get_data())
        timer.update()
