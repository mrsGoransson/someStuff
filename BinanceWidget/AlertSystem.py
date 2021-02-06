from threading import Lock

from SymbolTickerValues import SYMBOL_TICKER_VALUES
from Timer import CTimer


def _get_change_percentage_for_symbol(binance_response_data, symbol):
    if SYMBOL_TICKER_VALUES['price_change_percent'] in binance_response_data[symbol]:
        return float(binance_response_data[symbol][SYMBOL_TICKER_VALUES['price_change_percent']])
    return 0


class CAlertSystem:

    def __init__(self, alerter, alerts_enabled):
        self._alerter = alerter
        self._alert_data = {}
        self._alerts_enabled = alerts_enabled
        self._alerts_enabled_lock = Lock()
        self._resend_alert_duration = 60 * 15

    def alerts_enabled(self):
        with self._alerts_enabled_lock:
            return self._alerts_enabled

    def register_symbol(self, symbol, alert_value_up, alert_value_down):
        self._alert_data[symbol] = {'has_alerted': False, 'alert_value_up': alert_value_up,
                                    'alert_value_down': alert_value_down, 'alerted_at': 0}

    def toggle_alerts_enabled(self):
        with self._alerts_enabled_lock:
            self._alerts_enabled = not self._alerts_enabled
            if self._alerts_enabled:
                self._on_alerts_enabled()

    def update(self, binance_response_data):
        if self.alerts_enabled():
            for symbol in self._alert_data:
                percentage_change = _get_change_percentage_for_symbol(binance_response_data, symbol)
                if self._should_alert_user_for(symbol, percentage_change):
                    self._alert_change_to_user(symbol, percentage_change)
                elif self._alert_data[symbol]['has_alerted'] \
                        and (CTimer.get_current_time() - self._alert_data[symbol]['alerted_at']) \
                        > self._resend_alert_duration:
                    self._alert_data[symbol]['has_alerted'] = False

    def _alert_change_to_user(self, symbol_to_alert, change_percentage):
        print(symbol_to_alert + " " + str(change_percentage) + " change is being alerted to user")
        if change_percentage > 0:
            change = "increased"
        else:
            change = "decreased"

        text_message = "Value for: '" + symbol_to_alert \
                       + "' has " + change + " with " + str(change_percentage) + "%"

        self._alert_to_user(text_message)
        self._alert_data[symbol_to_alert]['has_alerted'] = True
        self._alert_data[symbol_to_alert]['alerted_at'] = CTimer.get_current_time()

    def _alert_to_user(self, message):
        self._alerter.alert(message)

    def _should_alert_user_for(self, symbol_to_evaluate, change_percentage):
        if not self._alert_data[symbol_to_evaluate]['has_alerted']:
            return (self._alert_data[symbol_to_evaluate]['alert_value_up'] != 0
                    and change_percentage > self._alert_data[symbol_to_evaluate]['alert_value_up']) \
                   or (self._alert_data[symbol_to_evaluate]['alert_value_down'] != 0
                       and change_percentage < self._alert_data[symbol_to_evaluate]['alert_value_down'])

        return False

    def _on_alerts_enabled(self):
        for symbol in self._alert_data.values():
            symbol['has_alerted'] = True
            symbol['alerted_at'] = CTimer.get_current_time()
