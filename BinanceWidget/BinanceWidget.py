import json
import sys
import PySimpleGUI
import time
import twilio.base

from os import path
from binance.client import Client as BinanceClient
from binance.websockets import BinanceSocketManager
from threading import Lock, Thread
from twilio.rest import Client as SmsClient
from twisted.internet import reactor

from SymbolTickerValues import SYMBOL_TICKER_VALUES


def _alert_change_to_user(symbol_to_alert, change_percentage):
    print(symbol_to_alert + " " + str(change_percentage) + " change is being alerted to user")
    if change_percentage > 0:
        change = "increased"
    else:
        change = "decreased"

    text_message = "Value for: " + symbol_to_alert \
                   + " has " + change + " with " + str(change_percentage) + "%"

    _send_text_message(text_message)
    alert_data[symbol_to_alert]['has_alerted'] = True
    alert_data[symbol_to_alert]['alerted_at'] = time.perf_counter()


def _alerts_enabled():
    with alert_enabled_lock:
        return alerts_enabled

    return False


def test(currently_enabled):
    currently_enabled = not currently_enabled
    return currently_enabled


def _create_and_run_graphical_window():
    window = _create_graphical_window()
    previous_response_data = {}
    new_binance_response_data = {}

    while True:
        event, values = window.read(timeout=10)
        if event == "_close" or event == PySimpleGUI.WIN_CLOSED:
            break
        elif event == '_toggle_alerts':
            with alert_enabled_lock:
                global alerts_enabled
                alerts_enabled = not alerts_enabled
                if alerts_enabled:
                    window['_toggle_alerts'].update(text='Disable Alert Messages', button_color=('black', 'orange'))
                else:
                    window['_toggle_alerts'].update(text='Enable Alert Messages', button_color=('white', 'blue'))

        if len(previous_response_data) > 0:
            first_key = list(previous_response_data.keys())[0]
            if new_binance_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']] > \
                    previous_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']]:
                window['_table'].update(values=_get_new_data_for_window_table(new_binance_response_data))

        with binance_data_lock:
            previous_response_data = new_binance_response_data.copy()
            new_binance_response_data = binance_response_data.copy()

        window.refresh()

    window.close()


def _create_binance_client():
    new_client = BinanceClient()
    return new_client


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
         PySimpleGUI.Button(button_text='Disable Alert Messages', key='_toggle_alerts',
                            button_color=('black', 'orange'))]]

    return PySimpleGUI.Window(title="Crypto Check v1.1", layout=layout, debugger_enabled=False, finalize=True)


def _create_binance_socket_manager():
    new_socket_manager = BinanceSocketManager(binance_client)
    return new_socket_manager


def _initialize_sms_service():
    sms_config_file = 'sms-config.json'
    if path.isfile(sms_config_file):
        with open(sms_config_file) as sms_config:
            try:
                sms_config_json = json.load(sms_config)
                new_sms_client = SmsClient(sms_config_json['account_sid'], sms_config_json['auth_token'])
            except json.decoder.JSONDecodeError:
                print("Error: Wrong format in " + sms_config_file)
                return [{}, {}]
    else:
        print("Error: Failed to find sms-config: " + sms_config_file)
        return [{}, {}]

    return [new_sms_client, sms_config_json]


def _get_change_percentage_for_symbol():
    with binance_data_lock:
        if SYMBOL_TICKER_VALUES['price_change_percent'] in binance_response_data[symbol]:
            return float(binance_response_data[symbol][SYMBOL_TICKER_VALUES['price_change_percent']])

    return 0


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


def _send_text_message(message):
    try:
         from_number = sms_config_data['from_phone_number']
         to_number = sms_config_data['to_phone_number']
         sms_client.messages.create(
            body=message,
            from_=from_number,
            to=to_number
         )
    except (twilio.base.exceptions.TwilioRestException, json.decoder.JSONDecodeError):
        print("Error: Can't send any text message to alert, check if sms-config is correct")


def _should_alert_user_for(symbol_to_evaluate, change_percentage):
    if not alert_data[symbol_to_evaluate]['has_alerted']:
        return (alert_data[symbol_to_evaluate]['alert_value_up'] != 0
                and change_percentage > alert_data[symbol_to_evaluate]['alert_value_up']) \
               or (alert_data[symbol_to_evaluate]['alert_value_down'] != 0
                   and change_percentage < alert_data[symbol_to_evaluate]['alert_value_down'])

    return False


def _start_trade_history_web_socket_for_symbol(symbol_to_start):
    connection_keys.append(binance_socket_manager.start_symbol_ticker_socket(symbol_to_start, _trade_history))


def _terminate_web_sockets():
    for key in connection_keys:
        binance_socket_manager.stop_socket(key)

    reactor.stop()


def _trade_history(message):
    if SYMBOL_TICKER_VALUES['event_type'] in message and SYMBOL_TICKER_VALUES['symbol'] in message:
        if message[SYMBOL_TICKER_VALUES['event_type']] == 'error':
            print("SERVER ERROR: for " + message[SYMBOL_TICKER_VALUES['symbol']])
        else:
            with binance_data_lock:
                binance_response_data[message[SYMBOL_TICKER_VALUES['symbol']]] = message


if __name__ == '__main__':
    print("Hello, don't close me unless you want to close the application!")
    start_time = time.perf_counter()

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

    connection_keys = []
    binance_response_data = {}
    alert_data = {}
    alerts_enabled = True
    binance_client = _create_binance_client()
    binance_socket_manager = _create_binance_socket_manager()

    for line in symbol_file_lines:
        [symbol_to_load, warning_value_up, warning_value_down] = _resolve_symbol_line(line)
        if len(symbol_to_load) < 1:
            continue

        binance_response_data[symbol_to_load] = {SYMBOL_TICKER_VALUES['event_time']: 0}
        alert_data[symbol_to_load] = {'has_alerted': False, 'alert_value_up': warning_value_up,
                                      'alert_value_down': warning_value_down, 'alerted_at': 0}

        _start_trade_history_web_socket_for_symbol(symbol_to_load)

    binance_data_lock = Lock()
    alert_enabled_lock = Lock()
    binance_socket_manager.start()

    [sms_client, sms_config_data] = _initialize_sms_service()
    if len(sms_config_data) < 4:
        print("Error: not enough data in sms-config")

    graphical_window_thread = Thread(target=_create_and_run_graphical_window, daemon=True)
    graphical_window_thread.start()
    resend_alert_duration = 60 * 15

    while True:
        frame_start_time = time.perf_counter()

        if not graphical_window_thread.is_alive():
            _terminate_web_sockets()
            sys.exit()

        if _alerts_enabled():
            for symbol in alert_data:
                percentage_change = _get_change_percentage_for_symbol()
                if _should_alert_user_for(symbol, percentage_change):
                    _alert_change_to_user(symbol, percentage_change)
                elif alert_data[symbol]['has_alerted'] \
                        and frame_start_time - alert_data[symbol]['alerted_at'] > resend_alert_duration:
                    alert_data[symbol]['has_alerted'] = False

        frame_time = time.perf_counter() - frame_start_time
        while frame_time < 0.017:
            frame_time = time.perf_counter() - frame_start_time
