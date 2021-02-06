from SymbolTickerValues import SYMBOL_TICKER_VALUES

from threading import Lock
import time


def _get_change_percentage_for_symbol(binance_data_lock, binance_response_data, symbol):
    with binance_data_lock:
        if SYMBOL_TICKER_VALUES['price_change_percent'] in binance_response_data[symbol]:
            return float(binance_response_data[symbol][SYMBOL_TICKER_VALUES['price_change_percent']])

    return 0


class CAlertSystem:

    def __init__(self, alerter, alerts_enabled):
        self._alerter = alerter
        self._alert_data = {}
        self._alerts_enabled = alerts_enabled
        self._alert_enabled_lock = Lock()
        self._resend_alert_duration = 60 * 15

    def __alerts_enabled__(self):
        with self._alert_enabled_lock:
            return self._alerts_enabled

    def __register_symbol__(self, symbol, alert_value_up, alert_value_down):
        self._alert_data[symbol] = {'has_alerted': False, 'alert_value_up': alert_value_up,
                                    'alert_value_down': alert_value_down, 'alerted_at': 0}

    def __toggle_alerts_enabled__(self):
        with self._alert_enabled_lock:
            self._alerts_enabled = not self._alerts_enabled

    def __update__(self, frame_start_time, binance_data_lock, binance_response_data):
        if not self.__alerts_enabled__():
            return
        for symbol in self._alert_data:
            percentage_change = _get_change_percentage_for_symbol(binance_data_lock, binance_response_data, symbol)
            if self._should_alert_user_for(symbol, percentage_change):
                self._alert_change_to_user(symbol, percentage_change)
            elif self._alert_data[symbol]['has_alerted'] \
                    and frame_start_time - self._alert_data[symbol]['alerted_at'] > self._resend_alert_duration:
                self._alert_data[symbol]['has_alerted'] = False

    def _alert_change_to_user(self, symbol_to_alert, change_percentage):
        print(symbol_to_alert + " " + str(change_percentage) + " change is being alerted to user")
        if change_percentage > 0:
            change = "increased"
        else:
            change = "decreased"

        text_message = "Value for: '" + symbol_to_alert \
                       + "' has " + change + " with " + str(change_percentage) + "%"

        self._alert_to_user__(text_message)
        self._alert_data[symbol_to_alert]['has_alerted'] = True
        self._alert_data[symbol_to_alert]['alerted_at'] = time.perf_counter()

    def _alert_to_user__(self, message):
        self._alerter.__alert__(message)

    def _should_alert_user_for(self, symbol_to_evaluate, change_percentage):
        if not self._alert_data[symbol_to_evaluate]['has_alerted']:
            return (self._alert_data[symbol_to_evaluate]['alert_value_up'] != 0
                    and change_percentage > self._alert_data[symbol_to_evaluate]['alert_value_up']) \
                   or (self._alert_data[symbol_to_evaluate]['alert_value_down'] != 0
                       and change_percentage < self._alert_data[symbol_to_evaluate]['alert_value_down'])

        return False
