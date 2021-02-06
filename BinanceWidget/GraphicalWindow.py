import PySimpleGUI

from SymbolTickerValues import SYMBOL_TICKER_VALUES


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


class CGraphicalWindow:

    def __init__(self, binance_manager, alert_system):
        self._binance_manager = binance_manager
        self._alert_system = alert_system
        self._alerts_button_data = {
            'colors': {'enabled': ('black', 'orange'), 'disabled': ('light goldenrod', 'green')},
            'texts': {'enabled': 'Disable Alert Messages', 'disabled': 'Enable Alert Messages'}}

    def start_and_run(self):
        window = self._create_graphical_window
        previous_response_data = {}
        new_binance_response_data = {}

        while True:
            event, values = window.read(timeout=10)
            if event == "_close" or event == PySimpleGUI.WIN_CLOSED:
                break
            elif event == '_toggle_alerts':
                self._alert_system.toggle_alerts_enabled()
                alerts_enabled = self._are_alerts_enabled
                window['_toggle_alerts'].update(text=self._get_alerts_button_text(alerts_enabled),
                                                button_color=self._get_alerts_button_colors(alerts_enabled))

            if len(previous_response_data) > 0:
                first_key = list(previous_response_data.keys())[0]
                if new_binance_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']] > \
                        previous_response_data[first_key][SYMBOL_TICKER_VALUES['event_time']]:
                    window['_table'].update(values=_get_new_data_for_window_table(new_binance_response_data))

            previous_response_data = new_binance_response_data.copy()
            new_binance_response_data = self._binance_manager.get_data()

            window.refresh()

        window.close()

    @property
    def _are_alerts_enabled(self):
        return self._alert_system.alerts_enabled()

    def _create_graphical_window(self):
        alerts_enabled = self._are_alerts_enabled

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
            {PySimpleGUI.Button(button_text="Close", key='_close'),
             PySimpleGUI.Button(
                 button_text=self._get_alerts_button_text(alerts_enabled),
                 button_color=self._get_alerts_button_colors(alerts_enabled),
                 key='_toggle_alerts')}]

        return PySimpleGUI.Window(title="Crypto Check v1.1", layout=layout, debugger_enabled=False, finalize=True)

    @property
    def _get_alerts_button_text(self, alerts_enabled):
        return self._alerts_button_data['texts']['enabled'] \
            if alerts_enabled else self._alerts_button_data['texts']['disabled']

    @property
    def _get_alerts_button_colors(self, alerts_enabled):
        return self._alerts_button_data['colors']['enabled'] \
            if alerts_enabled else self._alerts_button_data['colors']['disabled']
