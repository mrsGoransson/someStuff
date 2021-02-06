def resolve_symbol_line(symbol_line):
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
