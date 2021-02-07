from os import path


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


class CSymbolLoader:
    def __init__(self):
        self._loaded_symbols = []
        success = self._load('symbols.txt')
        if not success:
            print('Fatal Error, symbols not resolved')

    @property
    def get_loaded_symbols(self):
        return self._loaded_symbols

    def reload(self):
        self._loaded_symbols = []
        success = self._load('symbols.txt')
        if not success:
            print('Fatal Error, symbols not resolved')

    def _load(self, symbols_file_name):
        if path.isfile(symbols_file_name):
            with open(symbols_file_name) as symbols_file:
                symbol_file_lines = symbols_file.readlines()
        else:
            print("ERROR: Could not find file with symbols to load: " + symbols_file_name)
            input("Press 'enter' to exit...")
            return False

        if len(symbol_file_lines) < 1:
            print("ERROR: Failed to find symbols to load, file appear to be empty: " + symbols_file_name)
            input("Press 'enter' to exit...")
            return False

        for line in symbol_file_lines:
            [symbol_to_load, warning_value_up, warning_value_down] = _resolve_symbol_line(line)
            if len(symbol_to_load) < 1:
                continue
            self._loaded_symbols.append({
                'symbol': symbol_to_load, 'value_up': warning_value_up, 'value_down': warning_value_down})

        return True
